#ifndef MY_MOCK_LISTENER_HPP
#define MY_MOCK_LISTENER_HPP

#include "esp_log.h"
#include "AbstractListener.hpp"

static const char* mock_listener_TAG = "MOCK listener";

class MockListener : public AbstractListener
{
public:
    using AbstractListener::AbstractListener;

    void valueUpdated(const std::string& cfgName, const std::string& newValue) override
    {
        ESP_LOGI(mock_listener_TAG, "value updated, '%s':'%s'", cfgName.c_str(), newValue.c_str());
    }

    ~MockListener() {}
};

#endif