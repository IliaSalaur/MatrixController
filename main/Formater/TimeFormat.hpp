#ifndef WS_TIME_FORMAT_HPP
#define WS_TIME_FORMAT_HPP

#include "IFormat.hpp"
#include <string>
#include <ctime>


class TimeFormat : public IFormat
{
private:
    time_t m_timestamp;
    std::string _convertCharToTime(char c, tm& tmStruct)
    {
        switch (c)
        {
        case 's':
            return std::to_string(tmStruct.tm_sec);
        
        case 'm':            
            return tmStruct.tm_min < 10 ? "0" + std::to_string(tmStruct.tm_min) : std::to_string(tmStruct.tm_min);

        case 'h':
            return tmStruct.tm_hour < 10 ? "0" + std::to_string(tmStruct.tm_hour) : std::to_string(tmStruct.tm_hour);

        default:
            return std::string{c};
        }
    }

public:
    TimeFormat() : m_timestamp{0}{}
    const std::string format(const std::string& args) override
    {
        // localtime points to an internal object, so it is totally fine to use without clearing
        tm *time = localtime(&m_timestamp);

        std::string res{};
        for(const auto& c : args)
        {
            res += _convertCharToTime(c, *time);
        }

        return res;
    }

    void updateTimestamp(time_t timestamp)
    {
        m_timestamp = timestamp;
    }


};

#endif