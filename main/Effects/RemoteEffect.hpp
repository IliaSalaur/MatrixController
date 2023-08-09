#ifndef MY_REMOTE_EFFECT_HPP
#define MY_REMOTE_EFFECT_HPP

#include "freertos/task.h"
#include "freertos/queue.h"
#include <tasks_common.h>
#include "esp_log.h"

#include <esp_netif.h>
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#include "esp_http_client.h"

#include "wifi_app/MatrixInfo.hpp"
#include "AbstractEffect.hpp"
#include "nlohmann/json.hpp"

using nlohmann::json;
extern std::list<MatrixInfo> http_server_matrices;

constexpr size_t k_host_port = 8081;
static const char re_tag[] = "remote_effect";

class RemoteEffect : public AbstractEffect
{
private:
    TaskHandle_t m_taskHandle;

    static void _taskWrapper(void* pvParam)
    {
        RemoteEffect* effectPtr = static_cast<RemoteEffect*>(pvParam);
        uint8_t rx_buffer[effectPtr->m_fb->getWidth() * effectPtr->m_fb->getHeight() * sizeof(rgb_t) + 1]{};
        struct sockaddr_in dest_addr;

        while (1) {

            struct sockaddr_in *dest_addr_ip4 = &dest_addr;
            dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
            dest_addr_ip4->sin_family = AF_INET;
            dest_addr_ip4->sin_port = htons(k_host_port);

            int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
            if (sock < 0) {
                ESP_LOGE(re_tag, "Unable to create socket: errno %d", errno);
                break;
            }
            ESP_LOGI(re_tag, "Socket created");

            // Setting socket options
            struct timeval timeout;
            timeout.tv_sec = 60;
            timeout.tv_usec = 0;
            setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
            bool keepAlive{true};
            setsockopt (sock, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, sizeof(keepAlive));

            int err = bind(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
            if (err < 0) {
                ESP_LOGE(re_tag, "Socket unable to bind: errno %d", errno);
            }
            ESP_LOGI(re_tag, "Socket bound, port %d", k_host_port);
            

            while (1) {
                ESP_LOGI(re_tag, "Waiting for data");
                //blocks the execution
                int len = recv(sock, rx_buffer, sizeof( rx_buffer ) - 1, 0);

                // Error occurred during receiving
                if (len < 0) {
                    ESP_LOGE(re_tag, "recvfrom failed: errno %d", errno);
                    break;
                }
                // Data received
                else {
                    rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string...
                    ESP_LOGI(re_tag, "Received %d bytes:", len);

                    if(ulTaskNotifyTake(pdTRUE, portMAX_DELAY))  
                        effectPtr->m_fb->copyIntoFramebuffer(rx_buffer, len);
                }
            }

            if (sock != -1) {
                ESP_LOGE(re_tag, "Shutting down socket and restarting...");
                shutdown(sock, 0);
                close(sock);
            }
        }
    }

public:
    RemoteEffect(Framebuffer* fb) 
        : 
        AbstractEffect{fb, EffectsEnum::REMOTE_EFFECT}, 
        m_taskHandle{nullptr}
    {
        if(fb)
            xTaskCreate(RemoteEffect::_taskWrapper, "udp_server", 4096, this, 4, &this->m_taskHandle);
    }

    void setFrameBuffer(Framebuffer* fb) override
    {
        m_fb = fb;
        if(fb)
        {
            if(m_taskHandle)
                vTaskDelete(m_taskHandle);

            xTaskCreatePinnedToCore(
                RemoteEffect::_taskWrapper, 
                "udp_server", 
                UDP_SERVER_TASK_STACK_SIZE, 
                this, 
                UDP_SERVER_CHILD_TASK_PRIORITY, 
                &this->m_taskHandle,
                UDP_SERVER_CHILD_TASK_CORE_ID
                );
        }                
    }

    void setPropertiesFromMap(const effect_properties_t& props) override
    {

    }

    esp_err_t generateFrame() override
    {
        // Sets a lock/mutex
        m_fb->beginFrame();

        xTaskNotifyGive(this->m_taskHandle);

        return m_fb->endFrame();
    }

    ~RemoteEffect()
    {
        if(this->m_taskHandle)
            vTaskDelete(this->m_taskHandle);
    }
};
#endif