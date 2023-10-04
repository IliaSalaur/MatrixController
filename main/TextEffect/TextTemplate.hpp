#ifndef MY_TEXT_TEMPLATE_HPP
#define MY_TEXT_TEMPLATE_HPP

#include "Fonts.hpp"

#include "Effects/AbstractEffect.hpp"

#include "nlohmann/json.hpp"
using nlohmann::json;

static const char* TT_JS_TAG = "textTemplate_json";

struct TextTemplate
{    
    uint32_t letterCol;
    uint32_t backCol;
    size_t displayTime;
    size_t scrollTimes;
    const GFXfont* font;
    EffectsEnum textFilter;    
    std::string text;
    effect_properties_t textFilterProps;

    TextTemplate() : letterCol{0}, backCol{0}, displayTime{0}, scrollTimes{0}, font{Fonts::k_standard_font}, textFilter{EffectsEnum::NONE}, text{}, textFilterProps{}
    {

    }
    
    TextTemplate(uint32_t letterCol_, uint32_t backCol_, size_t displayTime_, size_t scrollTimes_, const GFXfont* font_, EffectsEnum textFilter_, std::string text_, effect_properties_t textFilterProperties_)
        :
        letterCol{letterCol_},
        backCol{backCol_},
        displayTime{displayTime_},
        scrollTimes{scrollTimes_},
        font{font_},
        textFilter{textFilter_},    
        text{std::move(text_)},
        textFilterProps{std::move(textFilterProperties_)}
    {}

    // NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(TextTemplate, letterCol, backCol, displayTime, scrollTimes, textFilter, text, textFilterProps)
    friend void to_json(nlohmann::json& nlohmann_json_j, const TextTemplate& nlohmann_json_t)
    {
        nlohmann_json_j["letterCol"] = nlohmann_json_t.letterCol;
        nlohmann_json_j["backCol"] = nlohmann_json_t.backCol;
        nlohmann_json_j["displayTime"] = nlohmann_json_t.displayTime;
        nlohmann_json_j["scrollTimes"] = nlohmann_json_t.scrollTimes;
        nlohmann_json_j["text"] = nlohmann_json_t.text;
        nlohmann_json_j["textFilter"] = nlohmann_json_t.textFilter;
        nlohmann_json_j["textFilterProps"] = nlohmann_json_t.textFilterProps;
        nlohmann_json_j["font"] = Fonts::getFontName(nlohmann_json_t.font);
    }

    friend void from_json(const nlohmann::json& nlohmann_json_j, TextTemplate& nlohmann_json_t)
    {
        //ESP_LOGI(TT_JS_TAG, "FROM_JSON");
        nlohmann_json_t.text = nlohmann_json_j.value("text", std::string{});
        //ESP_LOGI(TT_JS_TAG, "text: %s", nlohmann_json_t.text.c_str());

        nlohmann_json_t.letterCol = std::stoul(nlohmann_json_j.value("letterCol", std::string{"#0"}).substr(1), nullptr, 16);
        nlohmann_json_t.backCol = std::stoul(nlohmann_json_j.value("backCol", std::string{"#0"}).substr(1), nullptr, 16);

        //ESP_LOGI(TT_JS_TAG, "cols: %lu and %lu", nlohmann_json_t.letterCol, nlohmann_json_t.backCol);
        nlohmann_json_t.displayTime = nlohmann_json_j.value("displayTime", size_t{0});
        nlohmann_json_t.scrollTimes = (size_t)nlohmann_json_j.value("scrollTimes", size_t{0});
        nlohmann_json_t.textFilter = EffectsEnum{nlohmann_json_j.value("textFilter", 0)};

        // nlohmann_json_t.displayTime = static_cast<size_t>(nlohmann_json_j.value("displayTime", int{0}));
        // nlohmann_json_t.scrollTimes = static_cast<size_t>((size_t)nlohmann_json_j.value("scrollTimes", int{0}));
        // nlohmann_json_t.textFilter = EffectsEnum::NONE;
        // nlohmann_json_t.textFilter = EffectsEnum{nlohmann_json_j.value("textFilter", 0)};
        // nlohmann_json_t.textFilter = EffectsEnum{std::stoi(nlohmann_json_j.value("textFilter", "0"))};

        nlohmann_json_t.textFilterProps = effect_properties_t{};
        nlohmann_json_t.textFilterProps = nlohmann_json_j.value("textFilterProps", effect_properties_t{});

        nlohmann_json_t.font = Fonts::getFontFromName(nlohmann_json_j.value("font", ""));
        //ESP_LOGI(TT_JS_TAG, "done");
    }
};

#endif