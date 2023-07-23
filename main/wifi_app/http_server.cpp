#include <list>

#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_timer.h"

#include "http_server.h"
#include "http_webpage_handlers.hpp"
#include "tasks_common.h"
#include "wifi_app.h"
#include "esp_http_client.h"

#include "DynamicConfig/DynamicConfig.hpp"
#include "DynamicConfig/Loaders/NVSConfigLoader.hpp"

#include "Effects/EffectFactory.hpp"
#include "Effects/Animation.hpp"
#include "TextEffect/TextSequenceEffect.hpp"

#include "nlohmann/json.hpp"

using nlohmann::json;

static const char TAG[] = "http_server";

//extern wifi configs
extern WifiAppConfig wifi_app_config;

// extern config handle object
extern DynamicConfig conf;

// extern config loader
extern NVSConfigLoader loader;

extern QueueHandle_t g_animation_effects_queue;
extern QueueHandle_t g_child_matrices_queue;

// HTTP server task handle
static httpd_handle_t http_server_handle = NULL;

// list holding all matrices
std::list<MatrixInfo> http_server_matrices{};

void http_server_register_on_host()
{
    char local_response_buffer[k_http_server_data_buffer_size]{0};
    esp_http_client_config_t config = {
        .host = WIFI_APP_API_HOSTNAME,
        .path = "/registerDevice",
        .query = "esp",
        .event_handler = nullptr,
        .user_data = local_response_buffer,        // Pass address of local buffer to get response
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // POST
    json j = http_server_matrices.front();
    std::string data = j.dump();

    esp_http_client_set_url(client, "http://" WIFI_APP_API_HOSTNAME "/registerDevice");
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, data.c_str(), data.length());
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %" PRId64,
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
    }
    esp_http_client_cleanup(client);
}

/**
 * @brief POST handler requested when accessing the web page
 * @param req HTTP request for which the uri needs to be handled
 * @return ESP_OK if succeded
 */
extern "C" esp_err_t http_server_setTextTemplate_handler(httpd_req_t* req)
{
    ESP_LOGI(TAG, "setTextTemplate requested");
    char buf[k_http_server_data_buffer_size]{};

    size_t recv_size = MIN(req->content_len, k_http_server_data_buffer_size);
    ESP_LOGI("post", "LEN:%u, cutted:%u", req->content_len, recv_size);

    int ret = httpd_req_recv(req, buf, recv_size);
    if (ret <= 0) {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "POST data: |%s|", buf);
    json j = json::parse(buf);
    ESP_LOGI(TAG, "JSON Done");

    TextSequenceEffect* rawPtr = 
        new TextSequenceEffect{
            nullptr, 
            j["properties"].get<std::vector<TextTemplate>>()
        };
    ESP_LOGI(TAG, "setTextTemplate -> ptr addr:%p", rawPtr);

    if(
        xQueueSend(
            g_animation_effects_queue, 
            static_cast<void*>(&rawPtr), 
            1)  != pdPASS) {
      ESP_LOGW("post", "failed to send data to the queue in one tick");
    }
    
    httpd_resp_set_type(req, "text/html");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_send(req, "ok", 3);

    return ESP_OK;
}

extern "C" esp_err_t http_server_setEffect_handler(httpd_req_t* req)
{
    ESP_LOGI(TAG, "setEffect requested");

    char buf[k_http_server_data_buffer_size]{};

    size_t recv_size = MIN(req->content_len, k_http_server_data_buffer_size);
    ESP_LOGI("post", "LEN:%u, cutted:%u", req->content_len, recv_size);

    int ret = httpd_req_recv(req, buf, recv_size);
    if (ret <= 0) {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "POST data: |%s|", buf);
    json j = json::parse(buf);
    ESP_LOGI(TAG, "JSON Done");

    std::unique_ptr<AbstractEffect> ptr{EffectFactory::getEffect(nullptr, j)};

    AbstractEffect* rawPtr = ptr.get();

    // We've sent the raw pointer to AbstractEffect to the queue. If we don't release the ownership, std::unique_ptr will call a delete on that pointer    
    ptr.release();

    ESP_LOGI(TAG, "setEffect -> ptr addr:%p", rawPtr);

    if(
        xQueueSend(
            g_animation_effects_queue, 
            static_cast<void*>(&rawPtr), 
            1)  != pdPASS) {
      ESP_LOGW("post", "failed to send data to the queue in one tick");
    }
    
    httpd_resp_set_type(req, "text/html");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_send(req, "ok", 3);

    return ESP_OK;
}

extern "C" esp_err_t http_server_setConfigs_handler(httpd_req_t* req)
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
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_send(req, "ok", 3);

    return ESP_OK;
}

extern "C" esp_err_t http_server_registerDevice_handler(httpd_req_t* req)
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
        it->lastTimePing = esp_timer_get_time() / 1000;
    }
    else {
        ESP_LOGI(TAG, "Registering new device with id %lu and tag %s", newMatrix.id, newMatrix.tag.c_str());        
        http_server_matrices.push_back(std::move(newMatrix));
        http_server_matrices.back().lastTimePing = esp_timer_get_time() / 1000;
    }

    httpd_resp_set_type(req, "text/html");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_send(req, "ok", 3);

    return ESP_OK;
}

extern "C" esp_err_t http_server_addChild_handler(httpd_req_t* req)
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
    MatrixInfo* infoPtr = new MatrixInfo{j.get<MatrixInfo>()};

    if(xQueueSend(
        g_child_matrices_queue,
        static_cast<void*>(&infoPtr),
        1
    ) != pdPASS)
    {
        ESP_LOGW(TAG, "Unable to pass a MatrixInfo pointer to the queue");
    }

    httpd_resp_set_type(req, "text/html");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_send(req, "ok", 3);

    return ESP_OK;
}

extern "C" esp_err_t http_server_getConfigs_handler(httpd_req_t* req)
{    
    const json j = conf.getConfigs();
    const std::string s = j.dump();

    httpd_resp_set_type(req, "application/json");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_send(req, (const char*) s.c_str(), s.size());
    return ESP_OK;
}

extern "C" esp_err_t http_server_getDevices_handler(httpd_req_t* req)
{    
    ESP_LOGI(TAG, "get devices requested");

    // check timeouts
    http_server_matrices.remove_if([](const auto& item){
        return item.lastTimePing != -1 && esp_timer_get_time() / 1000 - item.lastTimePing > k_http_server_devices_timeout;
    });

    const json j = http_server_matrices;
    const std::string s = j.dump();

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, (const char*) s.c_str(), s.size());
    return ESP_OK;
}

extern "C" esp_err_t http_server_commitConfigs_handler(httpd_req_t* req)
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

extern "C" esp_err_t http_server_corsPreflight_handler(httpd_req_t* req)
{
    ESP_LOGI(TAG, "CORS Preflight requested");

    httpd_resp_set_type(req, "text/plain");
    // Setting response code 204 - No Content
    httpd_resp_set_status(req, HTTPD_204);
    // Satisfying CORS headers
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Headers", "*");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Methods", "POST, GET, OPTIONS, DELETE");
    httpd_resp_set_hdr(req, "Access-Control-Max-Age", "86400");

    httpd_resp_send(req, "", 1);
    return ESP_OK;
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
    config.max_uri_handlers = 40;

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
            &http_server_setEffect_handler,     // Handler function
            NULL                                // User context: NULL (not used)
        };
        httpd_register_uri_handler(http_server_handle, &setEffect);

        // register /setEffect(cors preflight OPTIONS) handler
        httpd_uri_t corsPreflightSetEffect{
            "/setEffect",                           // URI
            HTTP_OPTIONS,                           // Method: OPTIONS
            &http_server_corsPreflight_handler,     // Handler function
            NULL                                    // User context: NULL (not used)
        };
        httpd_register_uri_handler(http_server_handle, &corsPreflightSetEffect);

        // register /setTextTemplate handler
        httpd_uri_t setTextTemplate{
            "/setTextTemplate",                     // URI
            HTTP_POST,                              // Method: POST
            &http_server_setTextTemplate_handler,              // Handler function
            NULL                                    // User context: NULL (not used)
        };
        httpd_register_uri_handler(http_server_handle, &setTextTemplate);

        // register /setTextTemplate(cors preflight OPTIONS) handler
        httpd_uri_t corsPreflightSetTextTemplate{
            "/setTextTemplate",                     // URI
            HTTP_OPTIONS,                           // Method: OPTIONS
            &http_server_corsPreflight_handler,     // Handler function
            NULL                                    // User context: NULL (not used)
        };
        httpd_register_uri_handler(http_server_handle, &corsPreflightSetTextTemplate);

        // register /setConfigs handler
        httpd_uri_t setConfigs{
            "/setConfigs",                      // URI
            HTTP_POST,                          // Method: POST
            &http_server_setConfigs_handler,    // Handler function
            NULL                                // User context: NULL (not used)
        };
        httpd_register_uri_handler(http_server_handle, &setConfigs);

        // register /setConfigs(cors preflight OPTIONS) handler
        httpd_uri_t corsPreflightSetConfigs{
            "/setConfigs",                     // URI
            HTTP_OPTIONS,                           // Method: OPTIONS
            &http_server_corsPreflight_handler,     // Handler function
            NULL                                    // User context: NULL (not used)
        };
        httpd_register_uri_handler(http_server_handle, &corsPreflightSetConfigs);

        // register /getConfigs handler
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

        uint32_t esp_id = mac[2] | (uint32_t)mac[3] << 8 | (uint32_t)mac[4] << 16 | (uint32_t)mac[5] << 24;

        esp_netif_ip_info_t ip_info{};
        ESP_ERROR_CHECK(esp_netif_get_ip_info(wifi_app_config.enableAP ? esp_netif_ap : esp_netif_sta, &ip_info));        

        uint32_t ip = ip_info.ip.addr;
        // reverse the order of bytes
        ip = (ip & 0xff000000) >> 24 |  (ip & 0xff0000) >> 8 | (ip & 0xff00) << 8 | (ip & 0xff) << 24;
        
        http_server_matrices.push_front(MatrixInfo{
            esp_id,
            ip,
            std::stoul(conf.getConfig("width")),
            std::stoul(conf.getConfig("height")),
            -1,
            conf.getConfig("tag", wifi_app_config.enableAP ? "wsc-main" : std::string("wsc" + std::to_string(esp_id)).c_str())
        });

        ESP_LOGI(TAG, "MatrixInfo ip:%lu and id:%lu", ip, esp_id);
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