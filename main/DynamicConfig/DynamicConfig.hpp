#ifndef DYNAMIC_CONFIG_H
#define DYNAMIC_CONFIG_H

#include <map>
#include <string>
#include <vector>
#include <algorithm>

#include "Loaders/IConfigLoader.hpp"
#include "AbstractListener.hpp"

class DynamicConfig
{
private:
    std::map<const std::string, std::string> _cfgs;
    std::vector<AbstractListener*> _listeners;

    IConfigLoader* _loader;

    void _yieldListeners()
    {
        for(const auto& listener : _listeners)
        {
            for(const auto& cfgName : listener->getListenedConfigs())
            {
                if(const auto& it = _cfgs.find(cfgName); it != _cfgs.end())
                {
                    listener->valueUpdated(cfgName, it->second);
                }
            }
        }
    }
public:
    DynamicConfig(IConfigLoader* loader) : _cfgs(), _loader(loader){}    

    bool loadConfigs()
    {
        bool success = _loader->loadConfigs(_cfgs);
        if(!success) return false;
        this->_yieldListeners();
        return true;
    }

    const std::string& getConfig(const std::string& cfgName)
    {
        if(_cfgs.find(cfgName) != _cfgs.end()) return _cfgs[cfgName];
        return std::string{};
    }

    void saveConfig(const std::string& cfgName, const std::string&& cfgValue)
    {
        if(_cfgs[cfgName] == cfgValue) return;

        for(const auto& listener : _listeners)
        {
            const auto& listenSet = listener->getListenedConfigs();
            if(listenSet.find(cfgName) != listenSet.end())  listener->valueUpdated(cfgName, cfgValue);
        }

        _loader->saveConfig({cfgName, cfgValue});
        _cfgs[cfgName] = std::move(cfgValue);                 
    }

    const std::string& operator[](const std::string& cfgName){
        return this->getConfig(cfgName);
    }

    const std::map<const std::string, std::string>& getConfigs()
    {
        return this->_cfgs;
    }

    void attachListener(AbstractListener* listenerPtr)
    {
        if(!listenerPtr) return;
        
        _listeners.push_back(listenerPtr);

        for(const auto& cfgName : listenerPtr->getListenedConfigs())
            {
                if(const auto& it = _cfgs.find(cfgName); it != _cfgs.end())
                {
                    listenerPtr->valueUpdated(cfgName, it->second);
                }
            }
    }

    void detachListener(AbstractListener* listenerPtr)
    {
        _listeners.erase(std::remove(std::begin(_listeners), std::end(_listeners), listenerPtr), std::end(_listeners));
    }
};



#endif