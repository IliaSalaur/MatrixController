#ifndef MY_WIFI_LISTENER_HPP
#define MY_WIFI_LISTENER_HPP

#include "wifi_app.h"
#include "esp_log.h"
#include <DynamicConfig/AbstractListener.hpp>

static const char* TAG = "WiFi listener";
extern WifiAppConfig wifi_app_config;

class WifiConfigListener : public AbstractListener
{
public:
    WifiConfigListener() : AbstractListener{"ssid", "pass", "enableAP"}
    {}

    void valueUpdated(const std::string& cfgName, const std::string& newValue) override
    {
        ESP_LOGI(TAG, "value updated, '%s':'%s'", cfgName.c_str(), newValue.c_str());
        if(cfgName == "ssid")
        {
            wifi_app_config.ssid = newValue;
        }
        else if(cfgName == "pass")
        {
            wifi_app_config.pass = newValue;
        }
        else if(cfgName == "enableAP")
        {
            wifi_app_config.enableAP = std::stoi(newValue);
        }
    }

    ~WifiConfigListener() = default;
};

#endif