#ifndef MY_I_CONFIG_LOADER_HPP
#define MY_I_CONFIG_LOADER_HPP

#include <map>
#include <string>

class IConfigLoader{
protected:

public:
    virtual bool loadConfigs(std::map<const std::string, std::string>& configs) = 0;
    virtual bool saveConfig(const std::pair<std::string, std::string>& config) = 0;
    virtual ~IConfigLoader() = default;
};

#endif