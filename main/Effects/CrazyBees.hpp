#ifndef MY_CRAZYBEES_HPP
#define MY_CRAZYBEES_HPP

#include <cstdlib>

static constexpr size_t k_max_crazybees = 10; 

extern "C"
{
    #include <lib8tion.h>
    #include <noise.h>
};

#include "AbstractEffect.hpp"

class CrazyBees : public AbstractEffect
{
private:
    struct Bee
    {
        size_t x, y;
        size_t flower_x, flower_y;
        uint8_t hue;
    };

    uint8_t m_beesAmount;
    Bee m_bees[k_max_crazybees];

    void change_flower(uint8_t beeIndex)
    {
        m_bees[beeIndex].flower_x = random8_to(m_fb->getWidth());
        m_bees[beeIndex].flower_y = random8_to(m_fb->getHeight());
        m_bees[beeIndex].hue = random8();
    }

public:
    CrazyBees(Framebuffer* fb) : AbstractEffect{fb, EffectsEnum::CRAZYBEES}, m_beesAmount{}, m_bees{}
    {
        this->setScale(m_beesAmount);
    }

    void setPropertiesFromMap(const effect_properties_t& props) override
    {
        if(const auto it = props.find("scale"); it != props.end())
        {
            this->setScale(std::stoul(it->second));
        }
    } 

    uint8_t getScale()
    {
        return m_beesAmount;
    }

    void setScale(uint8_t scale)
    {
        m_beesAmount = scale;
        for (uint8_t i = 0; i < m_beesAmount; i++)
        {
            // set bee
            m_bees[i].x = random8_to(m_fb->getWidth());
            m_bees[i].y = random8_to(m_fb->getHeight());
            // set flower
            change_flower(i);
        }
    }

    esp_err_t generateFrame() override
    {
        // Sets a lock/mutex
        m_fb->beginFrame();

        static rgb_t white = { .r = 255, .g = 255, .b = 255 };

        m_fb->fade(8);
        for (uint8_t i = 0; i < m_beesAmount; i++)
        {
            // move bee
            if (m_bees[i].x > m_bees[i].flower_x) m_bees[i].x--;
            if (m_bees[i].y > m_bees[i].flower_y) m_bees[i].y--;
            if (m_bees[i].x < m_bees[i].flower_x) m_bees[i].x++;
            if (m_bees[i].y < m_bees[i].flower_y) m_bees[i].y++;

            // bingo, change flower
            if (m_bees[i].x == m_bees[i].flower_x && m_bees[i].y == m_bees[i].flower_y)
                change_flower(i);

            // draw bee
            m_fb->setPixel(m_bees[i].x, m_bees[i].y, white);

            // draw flower
            hsv_t c = { .h = m_bees[i].hue, .s = 255, .v = 255 };
            m_fb->setPixel(m_bees[i].flower_x - 1, m_bees[i].flower_y, c);
            m_fb->setPixel(m_bees[i].flower_x, m_bees[i].flower_y - 1, c);
            m_fb->setPixel(m_bees[i].flower_x + 1, m_bees[i].flower_y, c);
            m_fb->setPixel(m_bees[i].flower_x, m_bees[i].flower_y + 1, c);
        }
        m_fb->blur2d(16);

        // Releases the lock/mutex
        return m_fb->endFrame();
    }

    ~CrazyBees() = default;

};

#endif