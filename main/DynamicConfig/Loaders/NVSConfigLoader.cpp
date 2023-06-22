#include <memory>

#include "esp_err.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "nvs_handle.hpp"

#include "NVSConfigLoader.hpp"

const char* TAG = "nsv_conf_loader";
static constexpr size_t k_nvs_config_loader_buffer_size = 2048;

NVSConfigLoader::NVSConfigLoader(const std::string& ns) : m_nsName{ns}
{
}

bool NVSConfigLoader::init()
{
    // Init NVS
    esp_err_t ret = nvs_flash_init();
    if(ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    return ret == ESP_OK;
}

bool NVSConfigLoader::loadConfigs(std::map<const std::string, std::string> &configs)
{
    esp_err_t err{ESP_OK};
    // Open the nvs handle and perform some error checking
    std::unique_ptr<nvs::NVSHandle> handle = nvs::open_nvs_handle(m_nsName.c_str(), NVS_READONLY, &err);

    if(err != ESP_OK)
    {
        ESP_LOGW(TAG, "Error (%s) opening NVS handle", esp_err_to_name(err));
        return false;
    }

    nvs_iterator_t it = NULL;
    err = nvs_entry_find("nvs", m_nsName.c_str(), NVS_TYPE_ANY, &it);
    ESP_LOGI(TAG, "Iterator opening: (%s)", esp_err_to_name(err));
    while(err == ESP_OK)
    {
        nvs_entry_info_t info{};
        // Get the entry' info and log
        err = nvs_entry_info(it, &info); // Can omit error check if parameters are guaranteed to be non-NULL        
        ESP_LOGI(TAG, "key '%s', type '%d' \n", info.key, info.type);
        
        // Get the item from nvs and then emplace in the configs map
        char buf[k_nvs_config_loader_buffer_size]{};
        handle->get_string(info.key, buf, k_nvs_config_loader_buffer_size);  
        configs.emplace(info.key, buf);

        err = nvs_entry_next(&it);
    }

    if(err != ESP_ERR_INVALID_ARG)
    {
        nvs_release_iterator(it);
        return true;
    }  

    return false;
}

bool NVSConfigLoader::saveConfig(const std::pair<std::string, std::string> &config)
{
    const auto&[confName, confValue] = config;
    
    if(auto it = m_localCfgs.find(confName); it != m_localCfgs.end())
    {
        it->second = confValue;
        return true;
    }

    m_localCfgs.insert(config);
    return true;
}

esp_err_t NVSConfigLoader::commit()
{
    esp_err_t err{ESP_OK};
    // Open the nvs handle and perform some error checking
    std::unique_ptr<nvs::NVSHandle> handle = nvs::open_nvs_handle(m_nsName.c_str(), NVS_READWRITE, &err);

    if(err != ESP_OK)
    {
        ESP_LOGW(TAG, "Error (%s) opening NVS handle", esp_err_to_name(err));
        return err;
    }

    // Iterate through the map and save configs
    for (const auto&[confName, confValue] : m_localCfgs)
    {
        err = handle->set_string(confName.c_str(), confValue.c_str());
        if(err != ESP_OK)
        {
            ESP_LOGW(TAG, "Error (%s) saving the data, '%s':'%s'", esp_err_to_name(err), confName.c_str(), confValue.c_str());
            return err;
        }
    }

    err = handle->commit();
    if(err != ESP_OK)
    {
        ESP_LOGW(TAG, "Error (%s) commiting the data", esp_err_to_name(err));
    }

    return err;
}

NVSConfigLoader::~NVSConfigLoader() = default;