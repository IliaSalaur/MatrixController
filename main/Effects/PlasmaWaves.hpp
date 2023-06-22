#ifndef MY_PLASMA_WAVES_HPP
#define MY_PLASMA_WAVES_HPP

#include <cstdlib>

extern "C"
{
    #include <lib8tion.h>
};

static constexpr uint8_t exp_gamma[256] = {
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   1,   1,
    1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
    1,   2,   2,   2,   2,   2,   2,   2,   2,   2,   3,   3,   3,   3,   3,
    4,   4,   4,   4,   4,   5,   5,   5,   5,   5,   6,   6,   6,   7,   7,
    7,   7,   8,   8,   8,   9,   9,   9,   10,  10,  10,  11,  11,  12,  12,
    12,  13,  13,  14,  14,  14,  15,  15,  16,  16,  17,  17,  18,  18,  19,
    19,  20,  20,  21,  21,  22,  23,  23,  24,  24,  25,  26,  26,  27,  28,
    28,  29,  30,  30,  31,  32,  32,  33,  34,  35,  35,  36,  37,  38,  39,
    39,  40,  41,  42,  43,  44,  44,  45,  46,  47,  48,  49,  50,  51,  52,
    53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,  64,  65,  66,  67,
    68,  70,  71,  72,  73,  74,  75,  77,  78,  79,  80,  82,  83,  84,  85,
    87,  89,  91,  92,  93,  95,  96,  98,  99,  100, 101, 102, 105, 106, 108,
    109, 111, 112, 114, 115, 117, 118, 120, 121, 123, 125, 126, 128, 130, 131,
    133, 135, 136, 138, 140, 142, 143, 145, 147, 149, 151, 152, 154, 156, 158,
    160, 162, 164, 165, 167, 169, 171, 173, 175, 177, 179, 181, 183, 185, 187,
    190, 192, 194, 196, 198, 200, 202, 204, 207, 209, 211, 213, 216, 218, 220,
    222, 225, 227, 229, 232, 234, 236, 239, 241, 244, 246, 249, 251, 253, 254,
    255
};

#include "AbstractEffect.hpp"

class PlasmaWaves : public AbstractEffect
{
public:
    PlasmaWaves(Framebuffer* fb) : AbstractEffect{fb, EffectsEnum::PLASMAWAVES}, m_speed{}
    {
    }

    void setPropertiesFromMap(const effect_properties_t& props) override
    {
        if(const auto it = props.find("speed"); it != props.end())
        {
            m_speed = std::stoul(it->second);
        }
    } 

    uint8_t getSpeed()
    {
        return m_speed;
    }

    void setSpeed(uint8_t speed)
    {
        m_speed = scale8_video(256 - speed, 150);
        if(!m_speed) m_speed++;
    }

    esp_err_t generateFrame() override
    {
        // Sets a lock/mutex
        m_fb->beginFrame();

        uint8_t t1 = cos8((42 * m_fb->getFrameNumber()) / m_speed);
        uint8_t t2 = cos8((35 * m_fb->getFrameNumber()) / m_speed);
        uint8_t t3 = cos8((38 * m_fb->getFrameNumber()) / m_speed);

        for (uint16_t y = 0; y < m_fb->getHeight(); y++)
        {
            for (uint16_t x = 0; x < m_fb->getWidth(); x++)
            {
                // Calculate 3 separate plasma waves, one for each color channel
                uint8_t r = cos8((x << 3) + (t1 >> 1) + cos8(t2 + (y << 3)));
                uint8_t g = cos8((y << 3) + t1 + cos8((t3 >> 2) + (x << 3)));
                uint8_t b = cos8((y << 3) + t2 + cos8(t1 + x + (g >> 2)));
                m_fb->setPixel(x, y, (rgb_t){exp_gamma[r], exp_gamma[g], exp_gamma[b]});
            }
        }

        // Releases the lock/mutex
        return m_fb->endFrame();
    }

    ~PlasmaWaves() = default;

private:
    uint8_t m_speed;
};

#endif