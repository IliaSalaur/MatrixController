#ifndef TINY_JSON_HPP
#define TINY_JSON_HPP

#include <string.h>
#include <string>
#include <map>
#include <algorithm>

#ifdef DEBUG_TINY_JSON
#define JSON_Fm(x ...) Fm(x)
#else
#define JSON_Fm(x ...)
#endif

class TinyJson{
private:
    static bool validateJson(const char* json)
    {
        size_t len = strlen(json);

        size_t citates = 0, rBraces = 0, lBraces = 0;
        for(size_t i = 0; i < len; i++)
        {
            switch (json[i])
            {
            case '"':
                citates++;
                break;
            
            case '{':
                rBraces++;
                break;

            case '}':
                lBraces++;
                break;
            }
        }

        return !(citates % 2) && !((rBraces + lBraces) % 2);
    }
    
    template<typename ... T>
    static const std::tuple<size_t, std::string> findOneOf(const std::string& str, size_t idx = 0, T ... patterns)
    {
        size_t min = std::string::npos;
        std::string pat;
        
        ([&]
        {
          size_t i = str.find(patterns, idx);
          if(i < min)
          {
              min = i;
              pat = patterns;
          }
          
        }(), ...);
        return {min, pat};
    }
    
    static bool deserialize(std::string& jsonRaw, std::map<std::string, std::string>& jsonMap, size_t idx = 0, std::string pathPrefix = "/", bool isFirst = false)
    {
        static size_t i = 0;
        
        if(isFirst)
        {
            if(!validateJson(jsonRaw.c_str())) return 0;   
            i = 0;
        }
        
        i++;
        JSON_Fm("!!! i:%u, len:%u\n", i, jsonRaw.length())

        for(i = idx; i < jsonRaw.length();)
        {
            size_t kStartI = jsonRaw.find('"', i);
            if(kStartI == std::string::npos) 
                return 0;

            size_t kStopI = jsonRaw.find("\":", kStartI + 1);
            if(kStopI == std::string::npos)
                return 0;

            std::string key = std::string(jsonRaw.begin() + kStartI + 1, jsonRaw.begin() + kStopI);

            JSON_Fm("kStart:%u %c, kStop:%u %c\n", kStartI, jsonRaw[kStartI], kStopI, jsonRaw[kStopI])
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~            
            size_t vStartI = kStopI + 1;
            
            // auto foundResult = findOneOf(jsonRaw, vStartI, "\"}", "\",\"", "\",{", ",\"", "}");
            auto foundResult = jsonRaw[vStartI + 1] == '{' ? findOneOf(jsonRaw, vStartI, "}") : findOneOf(jsonRaw, vStartI, ",\"", "}\"", "}");
            size_t vStopI = std::get<0>(foundResult);
            if(vStopI == std::string::npos) return 0;
            
            if(vStopI != jsonRaw.length() && jsonRaw[vStartI + 1] == '{')
            {
                size_t nestEnd = vStopI;
                for(size_t j = vStartI + 2; j < nestEnd; j++)
                {
                    if(jsonRaw[j] == '{') vStopI++;
                }   
                foundResult = findOneOf(jsonRaw, vStopI, "}");
                vStopI = std::get<0>(foundResult) + 1;
            }

            std::string value = std::string(jsonRaw.begin() + vStartI + 1, jsonRaw.begin() + vStopI);

            JSON_Fm("vStart:%u %c, vStop:%u %c\n", vStartI, jsonRaw[vStartI], vStopI, jsonRaw[vStopI])

            JSON_Fm("pat: |%s| key: |%s|      value: |%s|\n", std::get<1>(foundResult).c_str(), key.c_str(), value.c_str())
            
            if(jsonRaw[vStartI + 1] == '{') deserialize(jsonRaw, jsonMap, vStartI + 1, pathPrefix + key + "/");
            else{
                value.erase(std::remove(value.begin(), value.end(), '"'), value.end());
                jsonMap.emplace(pathPrefix + std::move(key), std::move(value));
            }
            
            i = vStopI;
            JSON_Fm("i change:%u, char:|%c|\n\n", i, jsonRaw[i])
            if(jsonRaw[i] == '}') return true;
        }
        return false;
    }
    
public:

    static bool createJson(const char* json, std::map<std::string, std::string>& jsonMap, const std::string& defaultPath = "/")
    {
        std::string jsonStr(json);
        return deserialize(jsonStr, jsonMap, 0, defaultPath, true);
    }

};

#endif