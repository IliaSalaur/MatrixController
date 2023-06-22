#ifndef WS_ABSTRACT_LISTENER_H
#define WS_ABSTRACT_LISTENER_H

#include <string>
#include <set>

class AbstractListener
{
protected:
    std::set<std::string> _listenedConfigs;
public:
    AbstractListener(const std::initializer_list<std::string>& initList) : _listenedConfigs(initList) {}
    AbstractListener(const std::set<std::string>& listenedConfigs) : _listenedConfigs(listenedConfigs) {}
    AbstractListener() : _listenedConfigs() {}

    virtual void valueUpdated(const std::string& cfgName, const std::string& newValue) = 0;

    const std::set<std::string>&  getListenedConfigs()
    {
        return this->_listenedConfigs;
    }

    void addConfigToListener(const std::string& configName){
        _listenedConfigs.emplace(std::move(configName));
    }

    virtual ~AbstractListener() = default;
};

#endif