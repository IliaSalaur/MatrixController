#ifndef MY_SnowFall_HPP
#define MY_SnowFall_HPP

#include "AbstractEffect.hpp"

class SnowFall : public AbstractEffect
{
private:
    uint8_t m_scale;
    uint8_t m_speed;
public:
    SnowFall(Framebuffer* fb) : AbstractEffect{fb, EffectsEnum::SNOWFALL}, m_scale{10}, m_speed{40}
    {

    }

    void setPropertiesFromMap(const effect_properties_t& props) override
    {
        if(const auto it = props.find("scale"); it != props.end())
        {
            this->setScale(std::stoul(it->second));
        }

        if(const auto it = props.find("speed"); it != props.end())
        {
            this->setSpeed(std::stoul(it->second));
        }
    }

    uint8_t getSpeed()
    {
        return m_speed;
    }

    void setSpeed(uint8_t speed)
    {
        m_speed = speed < 70 ? 70 - speed : 1;
    }

    uint8_t getScale()
    {
        return m_scale;
    }

    void setScale(uint8_t scale)
    {
        m_scale = 256 - scale;
    }

    esp_err_t generateFrame() override
    {
        m_fb->beginFrame();

        // skip [m_speed]ms 
        static uint64_t tmr = esp_timer_get_time() / 1000;
        if(esp_timer_get_time() / 1000 - tmr < m_speed)
            return m_fb->endFrame();

        tmr += m_speed;       
        // Move everything down
        const size_t h = m_fb->getHeight();
        for(size_t x{0}; x < m_fb->getWidth(); x++)
        {
            for(size_t y{1}; y < m_fb->getHeight(); y++)
            {
                m_fb->setPixel(x, y - 1, m_fb->getPixel(x, y));
            }
        }

        // fill up the first row        
        for(size_t x{0}; x < m_fb->getWidth(); x++)
        {
            if(m_fb->getHEX(x, h - 2) == 0 && (random8_to(m_scale) == 0))
                m_fb->setPixel(x, h - 1, rgb_from_code(14745599 - 1052688 * random8_to(4)));
            else
                m_fb->setPixel(x, h - 1, rgb_t{0, 0, 0});
        }

        return m_fb->endFrame();
    }
};

#endif