#include "UdpMatrix.hpp"
#include <functional>

#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

constexpr size_t k_host_port = 8081;
static const char um_tag[] = "udp_matrix";

void UdpMatrix::_taskWrapper(void* pvParameter)
{
    UdpMatrix* matrixPtr{static_cast<UdpMatrix*>(pvParameter)};

    while (1) {

        struct sockaddr_in dest_addr;
        dest_addr.sin_addr.s_addr = inet_addr(matrixPtr->m_hostIP.c_str());
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(k_host_port);

        int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
        if (sock < 0) {
            ESP_LOGE(um_tag, "Unable to create socket: errno %d", errno);
            break;
        }

        // Set timeout
        struct timeval timeout;
        timeout.tv_sec = 10;
        timeout.tv_usec = 0;
        setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

        ESP_LOGI(um_tag, "Socket created, sending to %s:%d", matrixPtr->m_hostIP.c_str(), k_host_port);

        while (1) {

            int err = sendto(
                sock, 
                matrixPtr->m_pixels.data(), 
                matrixPtr->_width * matrixPtr->_height * sizeof(rgb_t), 
                0, 
                (struct sockaddr *)&dest_addr, 
                sizeof(dest_addr));            
            if (err < 0) {
                if(errno == ENOMEM)
                {
                    ESP_LOGW(um_tag, "Error NO MEM");
                    vTaskDelay(pdMS_TO_TICKS(50));
                }
                else
                {
                    ESP_LOGE(um_tag, "Error occurred during sending: errno %d", errno);
                    break;
                }                
            }
            else
                ESP_LOGI(um_tag, "Message sent");

            // struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
            // socklen_t socklen = sizeof(source_addr);
            // int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr *)&source_addr, &socklen);

            // // Error occurred during receiving
            // if (len < 0) {
            //     ESP_LOGE(um_tag, "recvfrom failed: errno %d", errno);
            //     break;
            // }
            // // Data received
            // else {
            //     rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
            //     ESP_LOGI(um_tag, "Received %d bytes from %s:", len, matrixPtr->m_hostIP.c_str());
            //     ESP_LOGI(um_tag, "%s", rx_buffer);
            // }

            vTaskDelay(pdMS_TO_TICKS(50));
            vTaskSuspend(NULL);
        }

        if (sock != -1) {
            ESP_LOGE(um_tag, "Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
            vTaskDelay(pdMS_TO_TICKS(5000));
        }
    }
}

UdpMatrix::UdpMatrix(uint8_t w, uint8_t h, std::string&& hostIP)
    :
    IMatrix{w, h},
    m_redrawRequired{true},
    m_brig{100},
    m_taskHandle{nullptr},
    m_pixels(w * h, rgb_t{0, 0, 0}),
    m_hostIP{hostIP}
{

}

void UdpMatrix::_redraw()
{
    if(m_redrawRequired)
        vTaskResume(this->m_taskHandle);
    m_redrawRequired = false;
}

void UdpMatrix::_drawPixel(int x, int y, rgb_t col)
{
    if(rgb_to_code(m_pixels[_xyToIndex(x, y)]) == rgb_to_code(col))
        return;

    m_pixels[_xyToIndex(x, y)] = col;
    m_redrawRequired = true;
}

size_t UdpMatrix::_xyToIndex(uint8_t x, uint8_t y)
{    
    return y * _width + x;
}

void UdpMatrix::begin()
{
    ESP_LOGI(um_tag, "Creating task...");
    xTaskCreatePinnedToCore(UdpMatrix::_taskWrapper, "udp_client", 4096, this, 4, &this->m_taskHandle, 0);
    ESP_LOGI(um_tag, "Task created! m_taskHandle = %p", &m_taskHandle);
}

void UdpMatrix::setBrightness(uint8_t brig)
{
}
