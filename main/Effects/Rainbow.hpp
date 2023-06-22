#ifndef MY_RAINBOW_HPP
#define MY_RAINBOW_HPP

#include "AbstractEffect.hpp"

class Rainbow : public AbstractEffect
{
public:
    enum direction_e{
        RAINBOW_HORIZONTAL = 0,
        RAINBOW_VERTICAL,
        RAINBOW_DIAGONAL,
    };

    Rainbow(Framebuffer* fb) : AbstractEffect{fb, EffectsEnum::RAINBOW}, m_direction{RAINBOW_HORIZONTAL}, m_scale{}, m_speed{}
    {

    }

    void setDirection(direction_e direction)
    {
        m_direction = direction;
    }

    void setScale(uint8_t scale){
        m_scale = scale; 
    }

    void setSpeed(uint8_t speed)
    {
        m_speed = speed;
    }

    uint8_t getScale()
    {
        return m_scale;
    }

    uint8_t getSpeed()
    {
        return m_speed;
    }
    
    direction_e getDirection()
    {
        return m_direction;
    }

    void setPropertiesFromMap(const effect_properties_t& props) override
    {
        if(const auto it = props.find("scale"); it != props.end())
        {
            m_scale = std::stoul(it->second);
        }

        if(const auto it = props.find("speed"); it != props.end())
        {
            m_speed = std::stoul(it->second);
        }

        if(const auto it = props.find("direction"); it != props.end())
        {
            m_direction = direction_e(std::stoul(it->second));
        }
    }    

    esp_err_t generateFrame() override
    {
        // Sets a lock/mutex
        m_fb->beginFrame();

        if (m_direction == RAINBOW_DIAGONAL)
        {
            for (size_t x = 0; x < m_fb->getWidth(); x++)
                for (size_t y = 0; y < m_fb->getHeight(); y++)
                {
                    float twirl = 3.0f * m_scale / 100.0f;
                    hsv_t color = {
                        .hue = uint8_t(m_fb->getFrameNumber() * m_speed * 2 + (m_fb->getWidth() / m_fb->getHeight() * x + y * twirl) * m_scale),
                        .sat = 255,
                        .val = 255
                    };
                    m_fb->setPixel(x, y, color);
                }
        }
        else
        {
            size_t outer = m_direction == RAINBOW_HORIZONTAL ? m_fb->getWidth() : m_fb->getHeight();
            size_t inner = m_direction == RAINBOW_HORIZONTAL ? m_fb->getHeight() : m_fb->getWidth();

            for (size_t i = 0; i < outer; i++)
            {
                hsv_t color = {
                    .hue = uint8_t(m_fb->getFrameNumber() * m_speed + i * m_scale),
                    .sat = 255,
                    .val = 255
                };
                for (size_t j = 0; j < inner; j++)
                    if (m_direction == RAINBOW_HORIZONTAL)
                        m_fb->setPixel(i, j, color);
                    else
                        m_fb->setPixel(j, i, color);
            }
        }

        // Releases the lock/mutex
        return m_fb->endFrame();
    }

    ~Rainbow() {}

private:    
    direction_e m_direction;
    uint8_t m_scale;
    uint8_t m_speed;
};

#endif