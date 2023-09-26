992#ifndef MY_SPARKLES_HPP
#define MY_SPARKLES_HPP

#include <cstdlib>

extern "C"
{
    #include <lib8tion.h>
    #include <noise.h>
};

#include "AbstractEffect.hpp"

class Sparkles : public AbstractEffect
{
public:
    Sparkles(Framebuffer* fb) : AbstractEffect{fb, EffectsEnum::SPARKLES}, m_maxSparkles{}, m_speed{}
    {

    }

    void setPropertiesFromMap(const effect_properties_t& props) override
    {
        if(const auto it = props.find("scale"); it != props.end())
        {
            m_maxSparkles = std::stoul(it->second);
        }

        if(const auto it = props.find("speed"); it != props.end())
        {
            m_speed = std::stoul(it->second);
        }
    } 

    void setScale(uint8_t scale)
    {
        m_maxSparkles = scale;
    }

    uint8_t getScale()
    {
        return m_maxSparkles;
    }

    void setSpeed(uint8_t speed)
    {
        m_speed = speed;
    }

    uint8_t getSpeed()
    {
        return m_speed;
    }

    esp_err_t generateFrame() override
    {
        // Sets a lock/mutex
        m_fb->beginFrame();

        m_fb->fade(8);
        for (uint8_t i = 0; i < m_maxSparkles; i++)
        {
            uint16_t x = random16_to(m_fb->getWidth());
            uint16_t y = random16_to(m_fb->getHeight());

            rgb_t c = m_fb->getPixel(x, y);

            if (rgb_luma(c) < 5)            
                m_fb->setPixel(x, y, hsv_from_values(random8(), 255, 255));
        }
        m_fb->fade(m_speed);

        // Releases the lock/mutex
        return m_fb->endFrame();
    }

    ~Sparkles() = default;

private:
    uint8_t m_maxSparkles;
    uint8_t m_speed;
};

#endif