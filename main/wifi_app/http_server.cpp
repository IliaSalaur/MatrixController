#include <list>

#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_timer.h"

#include "http_server.h"
#include "http_webpage_handlers.hpp"
#include "tasks_common.h"
#include "wifi_app.h"

#include "DynamicConfig/DynamicConfig.hpp"
#include "DynamicConfig/Loaders/NVSConfigLoader.hpp"

#include "nlohmann/json.hpp"

using nlohmann::json;

static const char TAG[] = "http_server";

//extern wifi configs
extern WifiAppConfig wifi_app_config;

// extern config handle object
extern DynamicConfig conf;

// extern config loader
extern NVSConfigLoader loader;

// Global Queue to handle incoming POST data for the /setEffect and /setTextTemplate
QueueHandle_t g_http_server_dataQueue;

// HTTP server task handle
static httpd_handle_t http_server_handle = NULL;

// list holding all matrices
static std::list<MatrixInfo> http_server_matrices{};

/**
 * @brief POST handler requested when accessing the web page
 * @param req HTTP request for which the uri needs to be handled
 * @return ESP_OK if succeded
 */
static esp_err_t http_server_post_handler(httpd_req_t* req)
{
    ESP_LOGI(TAG, "post requested");

    http_server_data data{};
    /* Truncate if content length larger than the buffer */
    size_t recv_size = MIN(req->content_len, sizeof(data.data));
    ESP_LOGI("post", "LEN:%u, cutted:%u", req->content_len, recv_size);

    int ret = httpd_req_recv(req, data.data, recv_size);
    if (ret <= 0) {  /* 0 return value indicates connection closed */
        /* Check if timeout occurred */
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            /* In case of timeout one can choose to retry calling
             * httpd_req_recv(), but to keep it simple, here we
             * respond with an HTTP 408 (Request Timeout) error */
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }

    strncpy(data.uri, req->uri, sizeof(data.uri));

    ESP_LOGI(TAG, "POST data: |%s|", data.data);
    // Send the data to the queue
    if (xQueueSend(g_http_server_dataQueue, static_cast<void*>(&data), 1) != pdPASS) {
      // Failed to send data in 1 tick, make a log
      ESP_LOGW("post", "failed to send data to the queue in one tick");
    }

    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, "ok", 3);

    return ESP_OK;
}

static esp_err_t http_server_setConfigs_handler(httpd_req_t* req)
{
    ESP_LOGI(TAG, "set config requested");

    char buf[k_http_server_data_buffer_size]{0};
    /* Truncate if content length larger than the buffer */
    size_t recv_size = MIN(req->content_len, k_http_server_data_buffer_size);
    ESP_LOGI("post", "LEN:%u, cutted:%u", req->content_len, recv_size);

    int ret = httpd_req_recv(req, buf, recv_size);
    if (ret <= 0) {  /* 0 return value indicates connection closed */
        /* Check if timeout occurred */
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            /* In case of timeout one can choose to retry calling
             * httpd_req_recv(), but to keep it simple, here we
             * respond with an HTTP 408 (Request Timeout) error */
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "POST data: |%s|", buf);

    // Parse the data to the json obj
    const json j = json::parse(buf);
    for(const auto&[confName, confValueJson] : j.items())
    {
        std::string confValue{};

        switch (confValueJson.type())
        {
        case json::value_t::string:
            confValue = confValueJson.get<std::string>();
            break;
        
        case json::value_t::boolean:
            confValue = std::to_string((int)confValueJson.get<bool>());
            break;

        case json::value_t::number_integer...json::value_t::number_unsigned:
            confValue = std::to_string(confValueJson.get<int>());            
            break;

        default:
            ESP_LOGI("post", "UNSUPORTED JSON VALUE, key: %s", confName.c_str());
            break;
        }
        
        conf.saveConfig(confName, std::move(confValue));
    }
    loader.commit();

    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, "ok", 3);

    return ESP_OK;
}

static esp_err_t http_server_registerDevice_handler(httpd_req_t* req)
{
    ESP_LOGI(TAG, "register device requested");

    char buf[k_http_server_data_buffer_size]{0};
    /* Truncate if content length larger than the buffer */
    size_t recv_size = MIN(req->content_len, k_http_server_data_buffer_size);
    ESP_LOGI("post", "LEN:%u, cutted:%u", req->content_len, recv_size);

    int ret = httpd_req_recv(req, buf, recv_size);
    if (ret <= 0) {  /* 0 return value indicates connection closed */
        /* Check if timeout occurred */
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            /* In case of timeout one can choose to retry calling
             * httpd_req_recv(), but to keep it simple, here we
             * respond with an HTTP 408 (Request Timeout) error */
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "POST data: |%s|", buf);

    // Parse the data to the json obj
    const json j = json::parse(buf);
    MatrixInfo newMatrix{j.get<MatrixInfo>()};
    
    // check if element already present
    auto it = std::find_if(
        std::begin(http_server_matrices), 
        std::end(http_server_matrices), 
        [&](const auto& item){
            return item.id == newMatrix.id;
        });

    if(it != http_server_matrices.end())
    {
        ESP_LOGI(TAG, "Device with id %lu found, updating lastTimePing", it->id);
        it->lastTimePing = esp_timer_get_time() * 1000;
    }
    else {
        ESP_LOGI(TAG, "Registering new device with id %lu and tag %s", newMatrix.id, newMatrix.tag.c_str());
        http_server_matrices.push_back(std::move(newMatrix));
    }

    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, "ok", 3);

    return ESP_OK;
}

static esp_err_t http_server_getConfigs_handler(httpd_req_t* req)
{    
    const json j = conf.getConfigs();
    const std::string s = j.dump();

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, (const char*) s.c_str(), s.size());
    return ESP_OK;
}

static esp_err_t http_server_getDevices_handler(httpd_req_t* req)
{    
    ESP_LOGI(TAG, "get devices requested");

    // check timeouts
    http_server_matrices.remove_if([](const auto& item){
        return item.lastTimePing != -1 && esp_timer_get_time() * 1000 - item.lastTimePing > k_http_server_devices_timeout;
    });

    const json j = http_server_matrices;
    const std::string s = j.dump();

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, (const char*) s.c_str(), s.size());
    return ESP_OK;
}

static esp_err_t http_server_commitConfigs_handler(httpd_req_t* req)
{
    ESP_LOGI(TAG, "commit requested");

    const auto res = loader.commit();
    if(res == ESP_OK)
    {
        httpd_resp_set_type(req, "text/plain");
        httpd_resp_send(req, "ok", 3);
        return ESP_OK;
    }

    httpd_resp_send_404(req);
    return res;
}

/**
 * @brief Sets up the default httpd server config
 * @return http server instance handle if successful, NULL otherwise 
 */
static httpd_handle_t http_server_configure(void)
{
    // Generate the default configuration
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    // The core that the HTTP server will run on
    config.core_id = HTTP_SERVER_TASK_CORE_ID;

    // Adjust the default priority
    config.task_priority = HTTP_SERVER_TASK_PRIORITY;

    // Bump up the stack size (default is 4096)
    config.stack_size = HTTP_SERVER_TASK_STACK_SIZE;

    // Increase uri handlers
    config.max_uri_handlers = 30;

    // Increase the timeout limit
    config.recv_wait_timeout = 10;
    config.send_wait_timeout = 10;



    ESP_LOGI(TAG, 
        "http_server_configure: starting server on port %d with task priority: %d", 
        config.server_port,
        config.task_priority);

    if(httpd_start(&http_server_handle, &config) == ESP_OK)
    {
        ESP_LOGI(TAG, "http_server_configure: Registering URI handlers");

        // register /setEffect handler
        httpd_uri_t setEffect{
            "/setEffect",                       // URI
            HTTP_POST,                          // Method: POST
            &http_server_post_handler,          // Handler function
            NULL                                // User context: NULL (not used)
        };
        httpd_register_uri_handler(http_server_handle, &setEffect);

        // register /setTextTemplate handler
        httpd_uri_t setTextTemplate{
            "/setTextTemplate",                     // URI
            HTTP_POST,                              // Method: POST
            &http_server_post_handler,              // Handler function
            NULL                                    // User context: NULL (not used)
        };
        httpd_register_uri_handler(http_server_handle, &setTextTemplate);

        // register /setConfigs handler
        httpd_uri_t setConfigs{
            "/setConfigs",                      // URI
            HTTP_POST,                          // Method: POST
            &http_server_setConfigs_handler,    // Handler function
            NULL                                // User context: NULL (not used)
        };
        httpd_register_uri_handler(http_server_handle, &setConfigs);

        // register /setConfigs handler
        httpd_uri_t getConfigs{
            "/getConfigs",                      // URI
            HTTP_GET,                           // Method: GET
            &http_server_getConfigs_handler,    // Handler function
            NULL                                // User context: NULL (not used)
        };
        httpd_register_uri_handler(http_server_handle, &getConfigs);

        // register /commitConfigs handler
        httpd_uri_t commitConfigs{
            "/commitConfigs",                   // URI
            HTTP_POST,                          // Method: POST
            &http_server_commitConfigs_handler, // Handler function
            NULL                                // User context: NULL (not used)
        };
        httpd_register_uri_handler(http_server_handle, &commitConfigs);

        // register /getDevices handler
        httpd_uri_t getDevices{
            "/getDevices",                      // URI
            HTTP_GET,                           // Method: GET
            &http_server_getDevices_handler,    // Handler function
            NULL                                // User context: NULL (not used)
        };
        httpd_register_uri_handler(http_server_handle, &getDevices);

        // register /registerDevice handler
        httpd_uri_t registerDevice{
            "/registerDevice",                      // URI
            HTTP_POST,                              // Method: POST
            &http_server_registerDevice_handler,    // Handler function
            NULL                                    // User context: NULL (not used)
        };
        httpd_register_uri_handler(http_server_handle, &registerDevice);

        http_server_register_webpage(http_server_handle);

        // Return the HTTP server handle
        return http_server_handle;
    }
    return NULL;
}

void http_server_start(void)
{
    if(http_server_handle == NULL)
    {
        http_server_handle = http_server_configure();
        
        uint8_t mac[6]{};
        ESP_ERROR_CHECK(esp_wifi_get_mac(wifi_app_config.enableAP ? WIFI_IF_AP : WIFI_IF_STA, mac));

        uint32_t esp_id = mac[0] | (uint32_t)mac[1] << 8 | (uint32_t)mac[2] << 16 | (uint32_t)mac[3] << 24;

        esp_netif_ip_info_t ip_info{};
        ESP_ERROR_CHECK(esp_netif_get_ip_info(wifi_app_config.enableAP ? esp_netif_ap : esp_netif_sta, &ip_info));

        http_server_matrices.push_front(MatrixInfo{
            esp_id,
            ip_info.ip.addr,
            -1,
            "wsc-main"
        });
    }
}

void http_server_stop(void)
{
    if(http_server_handle)
    {
        httpd_stop(http_server_handle);
        ESP_LOGI(TAG, "http_server_stop: stopping HTTP server");
    }
}