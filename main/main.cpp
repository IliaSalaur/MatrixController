/**
 * Application entry point.
*/

#include <string>
#include <map>

#include "Freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "nvs_flash.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "wifi_app/wifi_app.h"
#include "wifi_app/http_server.h"
#include "wifi_app/WifiConfigListener.hpp"

#include "Matrix/WS2812Matrix.hpp"
#include "Matrix/UdpMatrix.hpp"

#include "Effects/AbstractEffect.hpp"
#include "Effects/Animation.hpp"
#include "Effects/EffectFactory.hpp"

#include "nlohmann/json.hpp"

#include "DynamicConfig/DynamicConfig.hpp"
#include "DynamicConfig/Loaders/NVSConfigLoader.hpp"
#include "DynamicConfig/CallbackListener.hpp"


using nlohmann::json;

NVSConfigLoader loader{"storage"};
DynamicConfig conf{&loader};

// AnimationTask objects queue
QueueHandle_t g_animation_effects_queue;

// MatrixChilldTask objects queue
QueueHandle_t g_child_matrices_queue;

extern "C" void animationTask(void* pvParameter)
{
    ESP_LOGI("animation_task", "starting");
    IMatrix& matrix = *static_cast<IMatrix*>(pvParameter);

    Framebuffer fb{32, 16, std::bind(&IMatrix::renderer, static_cast<IMatrix*>(&matrix), std::placeholders::_1)};
    Animation anim{&fb, 60};
    std::unique_ptr<AbstractEffect> effect{EffectFactory::getEffect(&fb, EffectsEnum::FIRE)};
    g_animation_effects_queue = xQueueCreate(5, sizeof(AbstractEffect*));
    ESP_LOGI("animation_task", "started");
    while(1)
    {
        AbstractEffect* ptrBuf{nullptr};
        if(xQueueReceive(g_animation_effects_queue, &ptrBuf, portMAX_DELAY))
        {
            ESP_LOGI("animation_task", "setEffect -> ptr addr:%p", ptrBuf);
            if(!ptrBuf) continue;

            // Stop the animation, because effect is being used while transmiting pointer ownership
            anim.stop();
            effect.reset(ptrBuf);
            effect->setFrameBuffer(&fb);
            anim.play(effect.get());
        }
    }
}

extern "C" void matrixControlTask(void* pvParameter)
{
    conf.saveConfig("brig", "20", true);

    ESP_LOGI("matrix_control_task", "starting");
    WS2812Matrix& matrix = *static_cast<WS2812Matrix*>(pvParameter);

    matrix.begin();
    matrix.clearDisplay();

    conf.attachListener(new CallbackListener(
        {"brig"},
        [&](const std::string& cfgName, const std::string& newValue){
            if(cfgName == "brig")
                matrix.setBrightness(std::stoi(newValue));
        }        
    ));

    ESP_LOGI("matrix_control_task", "started");
    while (1)
    {
        vTaskDelay(1000);
    }
}

extern "C" void matrixChildTask(void* pvParameter)
{
    WS2812Matrix& matrix = *static_cast<WS2812Matrix*>(pvParameter);
    g_child_matrices_queue = xQueueCreate(5, sizeof(MatrixInfo*));

    while(1)
    {
        MatrixInfo *ptr{nullptr};
        if(xQueueReceive(g_child_matrices_queue, &ptr, portMAX_DELAY))
        {
            if(!ptr)
                continue;

            std::unique_ptr<MatrixInfo> childMatrixPtr{ptr};

            char ipBuf[32]{};
            esp_ip4_addr esp_ip{childMatrixPtr->ip};
            snprintf(ipBuf, 32, IPSTR, IP2STR(&esp_ip));

            ESP_LOGI("matrix_child_task", "IP of the child matrix:%s", ipBuf);

            IMatrix* childMatrix = new UdpMatrix{
                (uint8_t)childMatrixPtr->width, 
                (uint8_t)childMatrixPtr->height, 
                ipBuf};

            childMatrix->begin();
            matrix.addLeftChild(childMatrix);
        }
    }
}

extern "C" void app_main(void)
{
    loader.init();
    conf.attachListener(new WifiConfigListener{});
    conf.loadConfigs();

    // The Matrix object
    WS2812Matrix* matrix = new WS2812Matrix{
        (uint8_t)std::stoul(conf.getConfig("width")), 
        (uint8_t)std::stoul(conf.getConfig("height")), 
        GPIO_NUM_8};

    // Start WiFi
    wifi_app_start();

    // MatrixControlTask
    xTaskCreatePinnedToCore(
        &matrixControlTask,
        "matrix_control_task",
        8192,
        static_cast<void*>(matrix),
        3,
        NULL,
        0
    );

    // Animation Task
    xTaskCreatePinnedToCore(
        &animationTask,
        "anim_task",
        8192,
        static_cast<void*>(matrix),
        3,
        NULL,
        0
    );

    // MatrixChild Task
    xTaskCreatePinnedToCore(
        &matrixChildTask,
        "matrix_child_task",
        4096,
        static_cast<void*>(matrix),
        3,
        NULL,
        0
    );
}