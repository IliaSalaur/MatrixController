#ifndef WS_DATEFORMAT_HPP
#define WS_DATE_FORMAT_HPP

#include "IFormat.hpp"
#include <string>
#include <ctime>
#include <algorithm>


class DateFormat : public IFormat
{
private:
    time_t m_timestamp;
    std::string _convertCharToDate(char c, tm& tmStruct)
    {
        switch (c)
        {
        case 'd':
            return std::to_string(tmStruct.tm_mday);
        
        case 'm':
            return std::to_string(tmStruct.tm_mon + 1); // tm_mon counts months from January, so logically, the 3rd month, March will be counted as 2 months after January

        case 'y':
            return std::to_string(tmStruct.tm_year + 1900); // tm_year is years from 1900, so to get the current year we just add tm_year and 1900

        default:
            return std::string{c};
        }
    }

public:
    DateFormat() : m_timestamp{0}{}
    const std::string format(const std::string& args) override
    {
        // localtime points to an internal object, so it is totally fine to use without clearing
        tm *time = localtime(&m_timestamp);

        std::string res{};
        for(auto it = args.begin(); it != args.end();)
        {
            // Check if the char is coresponding to d[ay] / m[onth] / y[ear]
            if(*it != 'd' && *it != 'm' && *it != 'y')
            {
                res += *it;
                it++;
                continue;
            }
                
            // Get the char's occurence, so we could format the date as required by user, (yyyy - 2023, yy - 23) and (dd - 24, d - 4)
            size_t charsNum = std::count(it, std::end(args), *it);

            // Save the convertion's result into a temp
            const std::string& temp = this->_convertCharToDate(*it, *time);
            int pos = temp.length() - charsNum;

            // Substracting the required amount of chars from temp and concating our strings
            res += temp.substr(pos < 0 ? 0 : pos, charsNum);
            it += charsNum;            
        }

        return res;
    }

    void updateTimestamp(time_t timestamp)
    {
        m_timestamp = timestamp;
    }
};

#endif