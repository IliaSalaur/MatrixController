#ifndef MAIN_WIFI_APP_H_
#define MAIN_WIFI_APP_H_

#include <string>

#include "esp_netif.h"
#include "esp_wifi.h"
#include "Freertos/FreeRTOS.h"

// WIFI application settings
#define WIFI_AP_SSID               "WSC AP"         // AP name
#define WIFI_AP_PASSWORD           "password"       // AP password
#define WIFI_AP_CHANNEL            1                // AP channel
#define WIFI_AP_SSID_HIDDEN         0               // AP visibility
#define WIFI_AP_MAX_CONNECTIONS     5               // AP max clients
#define WIFI_AP_BEACON_INTERVAL     100             // AP beacon interval: 100 ms
#define WIFI_AP_IP                  "192.168.0.1"   // AP default IP
#define WIFI_AP_GATEWAY             "192.168.0.1"   // AP default Gateway 
#define WIFI_AP_NETMASK             "255.255.255.0" // AP netmask
#define WIFI_AP_BANDWIDTH           WIFI_BW_HT20    // AP bandwidth 20 MHz
#define WIFI_STA_POWER_SAVE         WIFI_PS_NONE    // Power save not used
#define MAX_SSID_LENGTH             32              // IEEE standard maximum
#define MAX_PASSWORD_LENGTH         64              // IEEE standard maximum
#define MAX_CONNECTION_RETRIES      5               // Retry number on disconnect
#define WIFI_APP_MDNS_HOSTNAME      "wsc"           // MDNS hostname
#define WIFI_APP_API_HOSTNAME       "wsc.local"     // API hostname

// netif object for the Station and Access Point
extern esp_netif_t* esp_netif_sta;
extern esp_netif_t* esp_netif_ap;

// config object
// struct WifiAppConfig{
//     std::string ssid{WIFI_AP_SSID};
//     std::string pass{WIFI_AP_PASSWORD};
//     bool enableAP{true};
// };

struct WifiAppConfig{
    std::string ssid{"Wokwi-GUEST"};
    std::string pass{""};
    bool enableAP{false};
};

/**
 * @brief Message IDs for the Wifi app task
 * 
 */
typedef enum wifi_app_message
{
    WIFI_APP_MSG_START_HTTP_SERVER = 0,
    WIFI_APP_MSG_CONNECTING_FROM_HTTP_SERVER,
    WIFI_APP_MSG_STA_CONNECTED_GOT_IP,
    WIFI_APP_MSG_START_STA,
    WIFI_APP_MSG_START_AP,
    WIFI_APP_MSG_REGISTER_ON_HOST,
    WIFI_APP_MSG_RECONNECT_WIFI,
    WIFI_APP_MSG_START_MDNS
} wifi_app_message_e;

/**
 * @brief Structure for the message queue
 * 
 */
typedef struct wifi_app_queue_message{
    wifi_app_message_e msgID;
} wifi_app_queue_message_t;

/**
 * @brief Sends message to the queue
 * @param msgID message ID from the wifi_app message_e enum.
 * @return pdTRUE if an item was successfully sent to the queue, otherwise pdFALSE
 */
BaseType_t wifi_app_send_message(wifi_app_message_e msgID);

/**
 * @brief Starts the WiFi RTOS task
 */
void wifi_app_start(void);
#endif