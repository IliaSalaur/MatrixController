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
#include "Matrix/UartMatrix.hpp"

#include "Effects/EffectFactory.hpp"
#include "TextEffect/TextSequenceEffect.hpp"

#include "nlohmann/json.hpp"

#include "DynamicConfig/DynamicConfig.hpp"
#include "DynamicConfig/Loaders/NVSConfigLoader.hpp"
#include "DynamicConfig/MockListener.hpp"


using nlohmann::json;

extern QueueHandle_t g_http_server_dataQueue;

NVSConfigLoader loader{"storage"};
DynamicConfig conf{&loader};


// The Matrix object
WS2812Matrix matrix(16, 16, GPIO_NUM_8);

extern "C" void ledTask(void* pvParams)
{
    WS2812Matrix matrix(16, 16, GPIO_NUM_8);
    matrix.begin();
    matrix.setBrightness(20);
    
    // matrix.begin();

    // UartMatrix ws{16, 16, UART_NUM_1, 460800};
    // ws.begin();
    // matrix.addLeftChild(&ws);



    Framebuffer fb{matrix.getTotalWidth(), 16, std::bind(&IMatrix::renderer, static_cast<IMatrix*>(&matrix), std::placeholders::_1)};
    Animation anim{&fb, 60};
    std::unique_ptr<AbstractEffect> effect = nullptr;//EffectFactory::getEffect(&fb, EffectsEnum::RAINBOW, {{"speed", "2"}, {"scale", "15"}});
    // anim.play(effect.get());

    http_server_data receivedData;

    while(1)
    {        
        // Run only when there is data in the queue
        if (xQueueReceive(g_http_server_dataQueue, &receivedData, portMAX_DELAY) == pdPASS) {
            // Parse the data as JSON
            const json j = json::parse(receivedData.data);

            if(strstr(receivedData.uri, "/setEffect"))
            {
                // Get the EffectsEnum from the EffectsMap
                const auto it = EffectsMap.find(j["effect"].get<std::string>());

                // If effect unknown - ignore
                if(it == EffectsMap.end()) continue;

                // If effect doesn't change, just change the props
                if(effect && it->second == effect->getEffect())
                {
                    ESP_LOGI("led_task", "Redrawing properties");
                    effect->setPropertiesFromMap(j["properties"].get<effect_properties_t>());
                }
                // Else get a new effect and re-run the animation
                else{
                    // At this point, the effect is guaranteed to be valid
                    effect = EffectFactory::getEffect(&fb, j);

                    // re-run the anim
                    anim.stop();
                    anim.play(effect.get());
                }
            } else if(strstr(receivedData.uri, "/setTextTemplate"))
            {                
                anim.stop();
                if(!effect || (effect && effect->getEffect() != EffectsEnum::TEXT_SEQUENCE))
                {
                    effect = std::make_unique<TextSequenceEffect>(&fb, std::vector<TextTemplate>{});
                }
                
                ESP_LOGI("led_task", "Effect ready to be set");

                TextSequenceEffect& seqEf = *static_cast<TextSequenceEffect*>(effect.get());

                ESP_LOGI("led_task", "Pointer set");
                std::vector<TextTemplate> seq{j.get<std::vector<TextTemplate>>()};
                ESP_LOGI("led_task", "got json");

                for(const auto&[lcol, bcol, dT, sT, tF, text, props] : seq)
                {
                    ESP_LOGI("led_task", "%lu, %lu, %u, %u, %s\n", 
                        lcol, 
                        bcol,
                        dT,
                        sT,
                        text.c_str());
                }

                ESP_LOGI("led_task", "Settings sequence");
                seqEf.setTextSequence(seq);
                ESP_LOGI("led_task", "Sequence set");

                // re-run the anim                
                anim.play(effect.get());
            }                            
        }
    }
}

extern "C" void app_main(void)
{
    loader.init();
    conf.attachListener(new MockListener{"test"});
    conf.attachListener(new WifiConfigListener{});
    conf.loadConfigs();    

    // Create a global queue to handle the incoming data from the /setEffect POST request
    g_http_server_dataQueue = xQueueCreate(k_http_server_dataQueue_size, k_http_server_data_size);

    // Start WiFi
    wifi_app_start();

    xTaskCreatePinnedToCore(
        &ledTask,
        "led_task",
        8192,
        NULL,
        3,
        NULL,
        0
    );
}