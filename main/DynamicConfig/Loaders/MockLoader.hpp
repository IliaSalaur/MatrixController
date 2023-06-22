#ifndef MY_MOCK_LISTENER_HPP
#define MY_MOCK_LISTENER_HPP

#include "IConfigLoader.hpp"

class MockConfigLoader : public IConfigLoader
{
private:
    std::map<const std::string, std::string> _mockCfgs;
public:
    MockConfigLoader(const std::initializer_list<std::pair<const std::string, std::string>>& initList) : _mockCfgs(initList){}

    bool loadConfigs(std::map<const std::string, std::string>& configs) override
    {
        for(const auto& p : _mockCfgs)
        {
            configs.emplace(p);
        }
        return true;
    }

    bool saveConfig(const std::pair<std::string, std::string>& config) override
    {
        _mockCfgs.emplace(config);

        return true;
    }
};

#endif