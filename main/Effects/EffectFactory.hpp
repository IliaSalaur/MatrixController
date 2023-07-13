#ifndef MY_EFFECT_FACTORY_HPP
#define MY_EFFECT_FACTORY_HPP

#include <memory>

#include "AbstractEffect.hpp"
#include "Framebuffer.hpp"
#include "Animation.hpp"
#include "Rainbow.hpp"
#include "Fire.hpp"
#include "Matrix.hpp"
#include "PlasmaWaves.hpp"
#include "CrazyBees.hpp"
#include "Waterfall.hpp"
#include "Sparkles.hpp"

#include "nlohmann/json.hpp"

const std::map<std::string_view, EffectsEnum> EffectsMap{
    {"Fire", EffectsEnum::FIRE},
    {"Matrix", EffectsEnum::MATRIX},
    {"Rainbow", EffectsEnum::RAINBOW},
    {"PlasmaWaves", EffectsEnum::PLASMAWAVES},
    {"CrazyBees", EffectsEnum::CRAZYBEES},
    {"Sparkles", EffectsEnum::SPARKLES},
    {"Waterfall", EffectsEnum::WATERFALL},
    {"TextSequence", EffectsEnum::TEXT_SEQUENCE}
};

class EffectFactory
{
public:
    static std::unique_ptr<AbstractEffect> getEffect(Framebuffer* fb, EffectsEnum effect)
    {
        switch (effect)
        {
        case EffectsEnum::FIRE: 
            return std::make_unique<Fire>(fb);
        
        case EffectsEnum::MATRIX:
            return std::make_unique<Matrix>(fb);

        case EffectsEnum::RAINBOW:
            return std::make_unique<Rainbow>(fb);

        case EffectsEnum::PLASMAWAVES:
            return std::make_unique<PlasmaWaves>(fb);

        case EffectsEnum::CRAZYBEES:
            return std::make_unique<CrazyBees>(fb);

        case EffectsEnum::SPARKLES:
            return std::make_unique<Sparkles>(fb);

        case EffectsEnum::WATERFALL:
            return std::make_unique<Waterfall>(fb);

        default:
            return nullptr;
        }
        return nullptr;
    }

    static std::unique_ptr<AbstractEffect> getEffect(Framebuffer* fb, EffectsEnum effect, const effect_properties_t& efProps, const std::string& propertiesPrefix = "")
    {        
        effect_properties_t localProps{};
        if(!propertiesPrefix.empty())
        {
            // Iterate through the efProps to create a new array of clean properties, without the prefix;
            for(const auto& [propName, propValue] : efProps)
            {
                ESP_LOGI("effect", "%s:%s", propName.c_str(), propValue.c_str());
                if(size_t index = propName.find(propertiesPrefix); index != std::string::npos)
                {                    
                    ESP_LOGI("effect", "changed to %s->%s", propName.c_str(), propName.substr(index + propertiesPrefix.length()).c_str());
                    localProps.emplace(propName.substr(index + propertiesPrefix.length()), propValue);
                }
            }
        }
        else localProps = efProps;

        std::unique_ptr<AbstractEffect> efPtr = EffectFactory::getEffect(fb, effect);
        if(efPtr) efPtr->setPropertiesFromMap(localProps);

        return efPtr;
    }

    static std::unique_ptr<AbstractEffect> getEffect(Framebuffer* fb, const nlohmann::json& j)
    {
        if(const auto it = EffectsMap.find(j["effect"].get<std::string>()); it != EffectsMap.end())
        {
            return getEffect(fb, it->second, j["properties"].get<effect_properties_t>());
        }
        return nullptr;
    }
};

#endif