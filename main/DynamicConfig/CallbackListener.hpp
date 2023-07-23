#ifndef MY_CALLBACK_LISTENER_HPP
#define MY_CALLBACK_LISTENER_HPP

#include <functional>
#include "AbstractListener.hpp"

typedef std::function<void(const std::string&, const std::string&)> listener_cb_t;

class CallbackListener : public AbstractListener
{
private:
    listener_cb_t m_callback;
public:
    
    CallbackListener(const std::initializer_list<std::string>& initList, listener_cb_t cb)
        : 
        AbstractListener(initList),
        m_callback{cb}
    {}

    void valueUpdated(const std::string& cfgName, const std::string& newValue) override
    {
        if(m_callback)
            m_callback(cfgName, newValue);
    }  
};

#endif