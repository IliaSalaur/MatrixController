#ifndef MY_FORMATER_HPP
#define MY_FORMATER_HPP

#include <string>
#include <map>
#include <algorithm>

#include "IFormat.hpp"

class Formater
{
private:
    std::map<std::string, IFormat*> m_formats;
public:
    Formater() = default;
    Formater(std::map<std::string, IFormat*> formats) : m_formats{formats}{}

    void addFormat(const std::string&& fmName, IFormat* fmPtr)
    {
        m_formats.emplace(fmName, fmPtr);
    }

    /**
     * @brief 
     * Method for formating a string.
     * Format:
     * "Some text {<formatName>#<formatArgs>}, and some more {<formatName>#<formatArgs>}".
     * Backslash in front of the curvy brace will make the method ignore the curve brace
     * @param str 
     * @return the formated std::string
     */
    std::string format(const std::string& str)
    {
        std::string rv = str;
        for(size_t i = 0; i < rv.length(); i++)
        {
            // If the current char isn't showing a format start, ignore and continue
            if(rv[i] != '{')   continue;

            // Find the pos of the format end, '}'
            size_t fmEnd = rv.find('}', ++i);
            if(fmEnd == std::string::npos)  return {"##fm error##"};
            // Find the pos of the format separator '#', that separates format's name and it's args
            size_t fmSeparator = rv.find('#', i);        

            // Obtain the name and args of the format
            const std::string fmName = rv.substr(i, fmSeparator == std::string::npos ? fmEnd - i: fmSeparator - i);
            // If there is no separator, fmArgs will be ""
            const std::string fmArgs = fmSeparator != std::string::npos ? rv.substr(++fmSeparator, fmEnd - fmSeparator) : "";

            // Erase the raw format
            rv.erase(std::begin(rv) + i - 1, std::begin(rv) + fmEnd + 1);

            // Find the IFormater that corresponds to fmName
            if(
                std::map<std::string, IFormat*>::const_iterator mapIt = m_formats.find(fmName); 
                mapIt != m_formats.end()
            )
            {
                const std::string& res = mapIt->second->format(fmArgs);                
                // Insert the formatted value 
                rv.insert(i - 1, res);
            }
#ifdef SHOW_FORMATER_ERRORS
            else{   // If not found, fm error will be inserted
                rv.insert(i - 1, "##fm error##");
            }
#endif
        }
        return rv;
    }
};

#endif