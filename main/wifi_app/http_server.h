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

/**
 * @brief 
 * 
 */
typedef struct http_server_data{
    char uri[CONFIG_HTTPD_MAX_URI_LEN + 1];
    char data[k_http_server_data_buffer_size];
} http_server_data_t;

constexpr size_t k_http_server_dataQueue_size = 1;
constexpr size_t k_http_server_data_size = sizeof(http_server_data_t);

struct MatrixInfo
{
    uint32_t id;
    uint32_t ip;
    // size_t width;
    // size_t height;  
    int64_t lastTimePing;  
    std::string tag;

    MatrixInfo()
        :
        id{0},
        ip{0},
        // width{width_},
        // height{height_},
        lastTimePing{-1},
        tag{""}
    {}

    MatrixInfo(uint32_t id_, uint32_t ip_, /*size_t width_, size_t height_,*/ int64_t lastPing_, const std::string& tag_)
        :
        id{id_},
        ip{ip_},
        // width{width_},
        // height{height_},
        lastTimePing{lastPing_},
        tag{tag_}
    {}

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MatrixInfo, id, ip, lastTimePing, tag)
};

/**
 * Messages for the HTTP monitor
 */
typedef enum http_server_message
{
    HTTP_MSG_WIFI_CONNECT_INIT = 0,
    HTTP_MSG_WIFI_CONNECT_SUCCESS,
    HTTP_MSG_WIFI_CONNECT_FAIL,
    HTTP_MSG_OTA_UPDATE_SUCCESSFUL,
    HTTP_MSG_OTA_UPDATE_FAILED,
    HTTP_MSG_OTA_UPDATE_INITIALIZED
} http_server_message_e;

/**
 * @brief Starts the HTTP server
 */
void http_server_start(void);

/**
 * @brief Stops the HTTP server
 */
void http_server_stop(void);
#endif