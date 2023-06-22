#ifndef WS_TIMETILL_FORMAT_HPP
#define WS_TIMETILL_FORMAT_HPP

#include "IFormat.hpp"
#include <string>
#include <string_view>
#include <ctime>

static constexpr std::string_view hoursInRussian[] {"час", "часа", "часов"};
static constexpr std::string_view daysInRussian[] {"день", "дня", "дней"};


class TimeTillFormat : public IFormat
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
            return std::to_string(tmStruct.tm_min);

        case 'h':
            return std::to_string(tmStruct.tm_hour);

        default:
            return std::string{c};
        }
    }

public:
    TimeTillFormat() : m_timestamp{0}{}
    const std::string format(const std::string& args) override
    {
        // Get the timeTill timestamp from args
        time_t timeTill = std::stoll(args);

        // If timeTill is lower than time now, return "0 hours"
        if(timeTill < m_timestamp)  return "0 часов";

        // now timeTill contains the difference
        timeTill -= m_timestamp;
        bool diffInDays = timeTill / (3600 * 24) >= 1;

        // Store the difference in res
        std::string res = std::to_string(diffInDays ? timeTill / (3600 * 24) : timeTill / 3600);
        res += " ";

        // Concat the coresponding time unit (day / days) and (hour / hours);
        switch(res[res.length() - 2])
        {
        case '1':   
            res += diffInDays ? daysInRussian[0] : hoursInRussian[0];
            break;
            
        case '2'...'4':
            res += diffInDays ? daysInRussian[1] : hoursInRussian[1];
            break;
            
        default:
            res += diffInDays ? daysInRussian[2] : hoursInRussian[2];
            break;
        }
        
        return res;
    }

    void updateTimestamp(time_t timestamp)
    {
        m_timestamp = timestamp;
    }


};

#endif