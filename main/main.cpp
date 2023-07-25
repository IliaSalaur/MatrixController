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

SemaphoreHandle_t g_conf_mutex{nullptr};

enum class AnimationTaskMsgEnum
{
    RESIZE_FB = 0
};

// AnimationTask objects queue
QueueHandle_t g_animation_effects_queue;
// AnimationTask msg queue
QueueHandle_t g_animation_msg_queue;

// MatrixChilldTask objects queue
QueueHandle_t g_child_matrices_queue;

// Semaphore for the matrix
SemaphoreHandle_t g_matrix_mutex;
WS2812Matrix* matrix{nullptr};

extern "C" void animationTask(void* pvParameter)
{
    ESP_LOGI("animation_task", "starting");
    Framebuffer fb{16, 16, std::bind(&WS2812Matrix::renderer, matrix, std::placeholders::_1)};

    Animation anim{&fb, 30};
    std::unique_ptr<AbstractEffect> effect{nullptr};

    g_animation_effects_queue = xQueueCreate(5, sizeof(AbstractEffect*));
    g_animation_msg_queue = xQueueCreate(5, sizeof(AnimationTaskMsgEnum));

    QueueSetHandle_t animQueueSet = xQueueCreateSet(10);
    xQueueAddToSet(g_animation_effects_queue, animQueueSet);
    xQueueAddToSet(g_animation_msg_queue, animQueueSet);

    ESP_LOGI("animation_task", "started");

    QueueSetMemberHandle_t xActivatedMember{nullptr};
    while(1)
    {
        xActivatedMember = xQueueSelectFromSet(animQueueSet, portMAX_DELAY);

        if(xActivatedMember == g_animation_msg_queue)
        {
            AnimationTaskMsgEnum msgEnum;
            xQueueReceive(xActivatedMember, &msgEnum, 0);

            switch (msgEnum)
            {
            case AnimationTaskMsgEnum::RESIZE_FB:
                {
                    // Needless to restart if isn't playing, otherwise will cause ESP_ERR_INVALID_STATE
                    bool restart = anim.isPlaying();
                    anim.stop();
                    fb.resize(
                        matrix->getTotalWidth(),
                        matrix->getTotalHeight());
                    if(restart)
                        anim.play(effect.get());
                }                
                break;
            
            default:
                break;
            }
        }
        else if(xActivatedMember == g_animation_effects_queue)
        {
            ESP_LOGI("animation_task", "receiving the ptr");
            AbstractEffect* ptrBuf{nullptr};
            xQueueReceive(xActivatedMember, &ptrBuf, 0);
        
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
    ESP_LOGI("matrix_control_task", "starting");    
    
    xSemaphoreTake(g_conf_mutex, portMAX_DELAY);

    conf.saveConfig("brig", "20", true);

    matrix = new WS2812Matrix{
    (uint8_t)std::stoul(conf.getConfig("width", "16")),
    (uint8_t)std::stoul(conf.getConfig("height", "16")),
    GPIO_NUM_8};    

    matrix->begin();
    matrix->clearDisplay();

    conf.attachListener(new CallbackListener(
        {"brig"},
        [&](const std::string& cfgName, const std::string& newValue){
            if(cfgName == "brig")
                matrix->setBrightness(std::stoi(newValue));
        }        
    ));

    xSemaphoreGive(g_conf_mutex);

    ESP_LOGI("matrix_control_task", "started");

    // Animation Task
    xTaskCreatePinnedToCore(
        &animationTask,
        "anim_task",
        8192,
        NULL,
        3,
        NULL,
        0
    );

    while (1)
    {
        vTaskDelay(1000);
    }
}

extern "C" void matrixChildTask(void* pvParameter)
{
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

            // xSemaphoreTake(g_matrix_mutex, portMAX_DELAY);  
            matrix->addLeftChild(childMatrix);
            // xSemaphoreGive(g_matrix_mutex);

            AnimationTaskMsgEnum msgEnum{AnimationTaskMsgEnum::RESIZE_FB};
            xQueueSend(g_animation_msg_queue, &msgEnum, 1);
        }
    }
}

extern "C" void app_main(void)
{
    loader.init();
    conf.attachListener(new WifiConfigListener{});
    conf.loadConfigs();

    ESP_LOGI("main", "getting width and height");
    // ESP_LOGI("main", "String values: w:|%s|  h:|%s|", conf.getConfig("width", "16").c_str(), conf.getConfig("height", "16").c_str());
    // ESP_LOGI("main", "int vals: %lu and %lu", std::stoul(conf.getConfig("width", "16")), std::stoul(conf.getConfig("height", "16")));    

    // ESP_LOGI("main", "matrix constructed: %p", matrix);

    g_conf_mutex = xSemaphoreCreateMutex();

    // Start WiFi
    wifi_app_start();

    // MatrixControlTask
    xTaskCreatePinnedToCore(
        &matrixControlTask,
        "matrix_control_task",
        8192,
        NULL,
        3,
        NULL,
        0
    );

    // MatrixChild Task
    xTaskCreatePinnedToCore(
        &matrixChildTask,
        "matrix_child_task",
        4096,
        NULL,
        3,
        NULL,
        0
    );
}