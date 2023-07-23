#ifndef MY_AbstractEffect_HPP
#define MY_AbstractEffect_HPP

#include <map>
#include <string>
#include "Framebuffer.hpp"

typedef std::map<std::string, std::string> effect_properties_t;

enum class EffectsEnum{
    FIRE = 0,
    MATRIX,
    RAINBOW,
    PLASMAWAVES,
    CRAZYBEES,
    SPARKLES,
    WATERFALL,
    SNOWFALL,

    REMOTE_EFFECT = 96,
    TEXT_SEQUENCE = 97,
    TEXT = 98,
    NONE = 99
};

/**
 * @brief Abstract class for generating effects
 * 
 */
class AbstractEffect
{
protected:
    Framebuffer* m_fb;
    EffectsEnum m_effect;
public:
    /**
     * @brief Construct a new AbstractEffect object
     * 
     * @param fb a pointer to the Framebuffer object
     * @param effect coresponding enum
     */
    AbstractEffect(Framebuffer* fb, EffectsEnum effect) : m_fb{fb}, m_effect{effect}
    {

    }

    /**
     * @brief Set the Frame Buffer object
     * 
     * @param fb a pointer to the Framebuffer object
     */
    virtual void setFrameBuffer(Framebuffer* fb)
    {
        m_fb = fb;
    }

    /**
     * @brief Get the coresponding EffectsEnum 
     * 
     * @return EffectsEnum 
     */
    EffectsEnum getEffect()
    {
        return m_effect;
    }

    /**
     * @brief Set the Properties of the Effect from a map
     * 
     * @param props key-value representation of the Effect's properties/parameters (such as scale, speed, mode, etc.)
     */
    virtual void setPropertiesFromMap(const effect_properties_t& props) = 0;    

    /**
     * @brief Generate the frame and fill the Framebuffer
     * 
     * @return esp_err_t result of Framebuffer::endFrame
     */
    virtual esp_err_t generateFrame() = 0; 

    /**
     * @brief Destroy the AbstractEffect object
     * 
     */
    virtual ~AbstractEffect() = default;
};

#endif