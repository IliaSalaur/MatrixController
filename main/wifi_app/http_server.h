 #ifndef MAIN_HTTP_SERVER_H_
#define MAIN_HTTP_SERVER_H_

#include "Freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

constexpr size_t k_http_server_data_buffer_size = sizeof(char[1024]);
constexpr size_t k_http_server_devices_timeout = 15000; //ms
constexpr size_t k_http_server_dataQueue_size = 1;

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
void http_server_pingHostTask_start(void);
#endif