#include "Freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"

#include "esp_err.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "lwip/netdb.h"
#include "mdns.h"

#include "nlohmann/json.hpp"

#include "tasks_common.h"
#include "wifi_app.h"
#include "http_server.h"

// Tag used for ESP serial console messages
static const char TAG[] = "wifi_app";

// Tag used for wifi_app's mdns messages
static const char mdnsTAG[] = "wifi_app mdns";

static bool wifi_app_isStarted = false;

// Queue handle used to manipulate the main queue of events
static QueueHandle_t wifi_app_queue_handle;

// Reconnect scenario should be handled differently. This variable shows if a reconnect was performed
static bool wifi_app_wasDisconnected = false;

// netif objects for the station and access point
esp_netif_t* esp_netif_sta = NULL;
esp_netif_t* esp_netif_ap = NULL;

WifiAppConfig wifi_app_config;

static bool query_mdns_host(const char *host_name)
{
    ESP_LOGI(mdnsTAG, "Query A: %s.local", host_name);

    struct esp_ip4_addr addr;
    addr.addr = 0;

    esp_err_t err = mdns_query_a(host_name, 2000,  &addr);//mdns_query(host_name, "_http", "_tcp", )
    if (err) {
        if (err == ESP_ERR_NOT_FOUND) {
            ESP_LOGW(mdnsTAG, "%s: Host was not found!", esp_err_to_name(err));
            return false;
        }
        ESP_LOGE(mdnsTAG, "Query Failed: %s", esp_err_to_name(err));
        return false;
    }

    ESP_LOGI(mdnsTAG, "Query A: %s.local resolved to: " IPSTR, host_name, IP2STR(&addr));
    return true;
}

bool start_mdns()
{
    // deinit mdns if was disconnected
    if(wifi_app_wasDisconnected)    mdns_free();
    // init mdns
    esp_err_t err = mdns_init();
    if(err){
        ESP_LOGE(mdnsTAG, "MDNS failed with code %d:%s", err, esp_err_to_name(err));
    }

    // query wsc.local - if not found - launch mdns service
    if(wifi_app_config.enableAP || !query_mdns_host(WIFI_APP_MDNS_HOSTNAME))
    {
        // set hostname
        ESP_ERROR_CHECK(mdns_hostname_set(WIFI_APP_MDNS_HOSTNAME));
        // set instance name
        ESP_ERROR_CHECK(mdns_instance_name_set(WIFI_APP_MDNS_HOSTNAME));

        // TXT records
        mdns_txt_item_t serviceTxtData[3]{
            {"board", "esp32"},
            {"u", "user"},
            {"p", "password"}
        };  

        // init service
        ESP_ERROR_CHECK( mdns_service_add(WIFI_APP_MDNS_HOSTNAME, "_http", "_tcp", 80, serviceTxtData, 3) );
        ESP_ERROR_CHECK( mdns_service_subtype_add_for_host(WIFI_APP_MDNS_HOSTNAME, "_http", "_tcp", NULL, "_server") );

        return true;
    }

    return false;
}

/**
 * @brief WiFi application event handler 
 * @param arg data, aside from event data, that is passed to the handler when it is called
 * @param event_base the base id of the event to register the handler for
 * @param event_id the id of the event to register the handler for
 * @param event_data event data
*/
static void wifi_app_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if(event_base == WIFI_EVENT)
    {
        switch (event_id)
        {
        case WIFI_EVENT_AP_START:
            ESP_LOGI(TAG, "EVENT -> AP Start");
            wifi_app_send_message(WIFI_APP_MSG_START_MDNS);
            // start http server
            wifi_app_send_message(WIFI_APP_MSG_START_HTTP_SREVER);            
            break;

        case WIFI_EVENT_AP_STOP:
            ESP_LOGI(TAG, "EVENT -> AP Stop");
            break;

        case WIFI_EVENT_AP_STACONNECTED:
            ESP_LOGI(TAG, "EVENT -> Station connected to AP");

        case WIFI_EVENT_AP_STADISCONNECTED:
            ESP_LOGI(TAG, "EVENT -> Station disconnected from AP");

        case WIFI_EVENT_STA_START:
            ESP_LOGI(TAG, "EVENT -> STA Start");
            if(!wifi_app_config.enableAP)ESP_ERROR_CHECK(esp_wifi_connect());            
            break;

        case WIFI_EVENT_STA_CONNECTED:
            ESP_LOGI(TAG, "EVENT -> STA connected to WiFi");
            break;
        
        case WIFI_EVENT_STA_DISCONNECTED:
            ESP_LOGI(TAG, "EVENT -> STA disconnected from WiFi");
            wifi_app_wasDisconnected = true;            

            if(!wifi_app_config.enableAP)   ESP_LOGI(TAG, "Reconnecting");
            if(!wifi_app_config.enableAP)   wifi_app_send_message(WIFI_APP_MSG_RECONNECT_WIFI);
            break;

        default:
            break;
        }
    } 
    else if(event_base == IP_EVENT)
    {
        switch (event_id)
        {
        case IP_EVENT_STA_GOT_IP:
            ESP_LOGI(TAG, "EVENT -> STA got IP Address");
            {   
                //start MDNS 
                wifi_app_send_message(WIFI_APP_MSG_START_MDNS);
                //start http server
                wifi_app_send_message(WIFI_APP_MSG_START_HTTP_SREVER);
                // if(!mdnsState)  wifi_app_send_message(WIFI_APP_MSG_REGISTER_ON_HOST);
            }
            break;
        
        default:
            break;
        }
    }
}

/**
 * @brief Inits the default TCP/IP stack and default WiFi configuration.
 */
static void wifi_app_default_init(void)
{
    // Init the TCP stack
    ESP_ERROR_CHECK(esp_netif_init());

    // Default WiFi config - operations must be in this order!
    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    esp_netif_sta = esp_netif_create_default_wifi_sta();
    esp_netif_ap = esp_netif_create_default_wifi_ap();
}

/**
 * @brief Configures the WiFi access point settings and assingns the static IP to the SoftAP.
 */
static void wifi_app_soft_ap_config(void)
{
    // SoftAP - WiFi access point configuration
    wifi_config_t ap_config{};
    strncpy((char*)ap_config.ap.ssid, wifi_app_config.ssid.c_str(), wifi_app_config.ssid.length() + 1);
    strncpy((char*)ap_config.ap.password, wifi_app_config.pass.c_str(), wifi_app_config.pass.length() + 1);
    ap_config.ap.ssid_len =  wifi_app_config.ssid.length();            
    ap_config.ap.channel = WIFI_AP_CHANNEL;
    ap_config.ap.authmode = WIFI_AUTH_WPA2_PSK;
    ap_config.ap.ssid_hidden = WIFI_AP_SSID_HIDDEN;
    ap_config.ap.max_connection = WIFI_AP_MAX_CONNECTIONS;
    ap_config.ap.beacon_interval = WIFI_AP_BEACON_INTERVAL;

    // Configure DHCP for the AP
    esp_netif_ip_info_t ap_ip_info{};

    esp_netif_dhcps_stop(esp_netif_ap);                 ///> must call this first
    inet_pton(AF_INET, WIFI_AP_IP, &ap_ip_info.ip);     ///> Assign access point's static IP, GW, and netmask
    inet_pton(AF_INET, WIFI_AP_GATEWAY, &ap_ip_info.gw);
    inet_pton(AF_INET, WIFI_AP_NETMASK, &ap_ip_info.netmask);
    ESP_ERROR_CHECK(esp_netif_set_ip_info(esp_netif_ap, &ap_ip_info));      ///> Statically configures the network interface
    ESP_ERROR_CHECK(esp_netif_dhcps_start(esp_netif_ap));                   ///> Start the AP DHCP server for connecting stations

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));                        ///> Setting the mode as Access Point / Station Mode
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));               ///> Set our configuration
    ESP_ERROR_CHECK(esp_wifi_set_bandwidth(WIFI_IF_AP, WIFI_AP_BANDWIDTH));    ///> Our default bandwidth 20 MHz
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_STA_POWER_SAVE));                      ///> Power save set to "NONE"
}

/**
 * @brief Configures the WIFI sta mode settings
 */
static void wifi_app_sta_config(void)
{
    // SoftAP - WiFi access point configuration
    wifi_config_t sta_config{};
    strncpy((char*)sta_config.sta.ssid, wifi_app_config.ssid.c_str(), wifi_app_config.ssid.length() + 1);
    strncpy((char*)sta_config.sta .password, wifi_app_config.pass.c_str(), wifi_app_config.pass.length() + 1);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));                        ///> Setting the mode as Access Point / Station Mode
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));               ///> Set our configuration
    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_STA_POWER_SAVE));  
}

static void wifi_app_restart(void)
{
    if(wifi_app_isStarted){
        // Stop WiFI
        ESP_ERROR_CHECK(esp_wifi_stop());    
    }

    // Start WiFI
    ESP_ERROR_CHECK(esp_wifi_start());
    wifi_app_isStarted = true;
    // Set the tx power to 8.5 dBm, otherwise won't work because of a hardware failure
    ESP_ERROR_CHECK(esp_wifi_set_max_tx_power(34));     ///> 34 corresponds to 8.5 dBm
}

/**
 * @brief Initializes the WiFi application event handler for WiFi and IP events 
 */
static void wifi_app_event_handler_init(void)
{
    // Event loop for the WiFi driver
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Event handler for the connection
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_app_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, &wifi_app_event_handler, NULL));    
}

/**
 * @brief Main task for the WiFI application
 * @param pvParams parameter which can be passed to the task
 */
static void wifi_app_task(void* pvParams)
{
    wifi_app_queue_message_t msg;

    // Init the event handler
    wifi_app_event_handler_init();

    // Init the TCP/IP stack and WiFi config
    wifi_app_default_init();

    wifi_app_send_message(wifi_app_config.enableAP ? WIFI_APP_MSG_START_AP : WIFI_APP_MSG_START_STA);


    for(;;)
    {
        if(xQueueReceive(wifi_app_queue_handle, &msg, portMAX_DELAY))
        {
            switch (msg.msgID)
            {
            case WIFI_APP_MSG_START_HTTP_SREVER:
                ESP_LOGI(TAG, "MSG -> Start HTTP Server");
                http_server_start();
                break;
            
            case WIFI_APP_MSG_CONNECTING_FROM_HTTP_SERVER:
                ESP_LOGI(TAG, "MSG -> Connecting from HTTP Server");
                break;

            case WIFI_APP_MSG_STA_CONNECTED_GOT_IP:
                ESP_LOGI(TAG, "MSG -> STA is connected and got IP");
                break;

            case WIFI_APP_MSG_START_AP:
                ESP_LOGI(TAG, "MSG -> Start Access Point");
                wifi_app_soft_ap_config();
                wifi_app_restart();                
                break;

            case WIFI_APP_MSG_START_STA:
                ESP_LOGI(TAG, "MSG -> Start STA");
                wifi_app_sta_config();
                wifi_app_restart();
                break;

            case WIFI_APP_MSG_REGISTER_ON_HOST:
                ESP_LOGI(TAG, "MSG -> Register on host");
                http_server_register_on_host();
                break;

            case WIFI_APP_MSG_RECONNECT_WIFI:
                ESP_LOGI(TAG, "MSG -> WiFi Reconnect");
                if(!wifi_app_config.enableAP) vTaskDelay(pdMS_TO_TICKS(1500));
                ESP_ERROR_CHECK(esp_wifi_connect()); 
                break;

            case WIFI_APP_MSG_START_MDNS:
                ESP_LOGI(TAG, "MSG -> Starting MDNS");
                if(!start_mdns()) wifi_app_send_message(WIFI_APP_MSG_REGISTER_ON_HOST);

                break;
            default:
                break;
            }
        }
    }
}

BaseType_t wifi_app_send_message(wifi_app_message_e msgID)
{
    wifi_app_queue_message_t msg;
    msg.msgID = msgID;
    return xQueueSend(wifi_app_queue_handle, &msg, portMAX_DELAY);
}

void wifi_app_start(void)
{
    ESP_LOGI(TAG, "STARTING WIFI APPLICATION");

    // Disable default WiFi logging level
    esp_log_level_set("wifi", ESP_LOG_NONE);

    // Create message queue
    wifi_app_queue_handle = xQueueCreate(5, sizeof(wifi_app_queue_message_t));

    // Start the WiFi application
    xTaskCreatePinnedToCore(&wifi_app_task, "wifi_app_task", WIFI_APP_TASK_STACK_SIZE, NULL, WIFI_APP_TASK_PRIORITY, NULL, WIFI_APP_TASK_CORE_ID);
}