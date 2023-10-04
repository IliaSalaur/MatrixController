#ifndef FONT_ONE_HEADER_INCLUDE_HPP
#define FONT_ONE_HEADER_INCLUDE_HPP

#include "GFXFont.hpp"
#include <map>
#include <string_view>

namespace Fonts
{    
    #include "FontsRus/Bahamas6.h"
    #include "FontsRus/Cooper6.h"
    #include "FontsRus/CourierCyr6.h"
    #include "FontsRus/Crystal6.h"
    #include "FontsRus/CrystalNormal6.h"
    #include "FontsRus/FreeMono6.h"
    #include "FontsRus/FreeMonoBold6.h"
    #include "FontsRus/FreeMonoBoldOblique6.h"
    #include "FontsRus/FreeMonoOblique6.h"
    #include "FontsRus/FreeSans6.h"
    #include "FontsRus/FreeSansBold6.h"
    #include "FontsRus/FreeSansBoldOblique6.h"
    #include "FontsRus/FreeSansOblique6.h"
    #include "FontsRus/FreeSerif6.h"
    #include "FontsRus/FreeSerifBold6.h"
    #include "FontsRus/FreeSerifBoldItalic6.h"
    #include "FontsRus/FreeSerifItalic6.h"
    #include "FontsRus/TimesNRCyr6.h"

    const std::map<std::string_view, const GFXfont*> FontsMap{
        {"Bahamas", &Bahamas6pt8b},
        {"Cooper", &Cooper6pt8b},
        {"CourierCyr", &CourierCyr6pt8b},
        {"Crystal", &Crystal6pt8b},
        {"CrystalNormal", &CrystalNormal6pt8b},
        {"FreeMono", &FreeMono6pt8b},
        {"FreeMonoBold", &FreeMonoBold6pt8b},
        {"FreeMonoBoldOblique", &FreeMonoBoldOblique6pt8b},
        {"FreeMonoOblique", &FreeMonoOblique6pt8b},
        {"FreeSans", &FreeSans6pt8b},
        {"FreeSansBold", &FreeSansBold6pt8b},
        {"FreeSansBoldOblique", &FreeSansBoldOblique6pt8b},
        {"FreeSansOblique", &FreeSansOblique6pt8b},
        {"FreeSerif", &FreeSerif6pt8b},
        {"FreeSerifBold", &FreeSerifBold6pt8b},
        {"FreeSerifBoldItalic", &FreeSerifBoldItalic6pt8b},
        {"FreeSerifItalic", &FreeSerifItalic6pt8b},
        {"TimesNRCyr", &TimesNRCyr6pt8b},
    };

    const GFXfont* k_standard_font = &Fonts::FreeMono6pt8b;

    std::string getFontName(const GFXfont* font)
    {
        for(const auto& [name, fontPtr] : FontsMap)
        {
            if(fontPtr == font)
                return std::string(name);
        }

        return "";
    }

    const GFXfont* getFontFromName(const std::string& name)
    {
        if(const auto it = FontsMap.find(name); it != FontsMap.end())
            return it->second;
        
        return k_standard_font;
    }
};

#endif