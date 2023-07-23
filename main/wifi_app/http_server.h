 #ifndef MAIN_HTTP_SERVER_H_
#define MAIN_HTTP_SERVER_H_

#include <cstdint>
#include <string>

#include "Freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "nlohmann/json.hpp"

constexpr size_t k_http_server_data_buffer_size = sizeof(char[1024]);
constexpr size_t k_http_server_devices_timeout = 15000; //ms
constexpr size_t k_http_server_dataQueue_size = 1;

struct MatrixInfo
{
    uint32_t id{0};
    uint32_t ip{0};
    size_t width;
    size_t height;  
    int64_t lastTimePing{-1};  
    std::string tag{};

    MatrixInfo()
        :
        id{0},
        ip{0},
        width{0},
        height{0},
        lastTimePing{-1},
        tag{""}
    {}

    MatrixInfo(uint32_t id_, uint32_t ip_, size_t width_, size_t height_, int64_t lastPing_, const std::string& tag_)
        :
        id{id_},
        ip{ip_},
        width{width_},
        height{height_},
        lastTimePing{lastPing_},
        tag{tag_}
    {}

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MatrixInfo, id, ip, width, height, tag)
};

/**
 * @brief Starts the HTTP server
 */
void http_server_start(void);

/**
 * @brief Stops the HTTP server
 */
void http_server_stop(void);

/**
 * @brief Sends a register request to the host, containing MatrixInfo
 */
void http_server_register_on_host(void);
#endif