#ifndef MY_NVS_CONFIG_LOADER_HPP
#define MY_NVS_CONFIG_LOADER_HPP

#include <unordered_map>

#include "IConfigLoader.hpp"

class NVSConfigLoader : public IConfigLoader
{
private:
    std::unordered_map<std::string, std::string> m_localCfgs;
    std::string m_nsName;
public:
    NVSConfigLoader(const std::string& ns);

    bool init();
    bool loadConfigs(std::map<const std::string, std::string>& configs) override;
    bool saveConfig(const std::pair<std::string, std::string>& config) override;
    esp_err_t commit();

    ~NVSConfigLoader();
};
#endif