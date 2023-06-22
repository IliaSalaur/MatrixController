#ifndef MY_MATRIX_HPP
#define MY_MATRIX_HPP

#include <cstdlib>

#define MATRIX_START_COLOR   0x9bf800
#define MATRIX_DIM_COLOR     0x558800
#define MATRIX_STEP          0x0a1000
#define MATRIX_ALMOST_OFF    0x050800
#define MATRIX_OFF_THRESH    0x030000
#define MATRIX_DIMMEST_COLOR 0x020300

extern "C"
{
    #include <lib8tion.h>
    #include <noise.h>
};

#include "AbstractEffect.hpp"

class Matrix : public AbstractEffect
{
public:
    Matrix(Framebuffer* fb) : AbstractEffect{fb, EffectsEnum::MATRIX}, m_density{}
    {

    }

    void setPropertiesFromMap(const effect_properties_t& props) override
    {
        if(const auto it = props.find("density"); it != props.end())
        {
            m_density = std::stoul(it->second);
        }
    } 

    uint8_t getDensity()
    {
        return m_density;
    }

    void setDensity(uint8_t density)
    {
        m_density = 255 - density;
    }

    esp_err_t generateFrame() override
    {
        // Sets a lock/mutex
        m_fb->beginFrame();

        for (size_t x = 0; x < m_fb->getWidth(); x++)
        {
            // process matrix from bottom to the second line from the top
            for (size_t y = 0; y < m_fb->getHeight() - 1; y++)
            {
                // get current pixel color
                rgb_t cur_color = m_fb->getPixel(x, y);
                // get color of the pixel above current
                rgb_t upper_color = m_fb->getPixel(x, y + 1);

                uint32_t cur_code = rgb_to_code(cur_color);                
                uint32_t upper_code = rgb_to_code(upper_color);

                // if above is max brightness, ignore this fact with some probability or move tail down
                if (upper_code == MATRIX_START_COLOR && random8_to(7 * m_fb->getHeight()) != 0)
                    m_fb->setPixel(x, y, upper_color);
                // if current pixel is off, light up new tails with some probability
                else if (cur_code == 0 && random8_to(m_density) == 0)
                    m_fb->setPixel(x, y, rgb_from_code(MATRIX_START_COLOR));
                // if current pixel is almost off, try to make the fading out slower
                else if (cur_code <= MATRIX_ALMOST_OFF)
                {
                    if (cur_code >= MATRIX_OFF_THRESH)
                        m_fb->setPixel(x, y, rgb_from_code(MATRIX_DIMMEST_COLOR));
                    else if (cur_code != 0)
                        m_fb->setPixel(x, y, rgb_from_code(0));
                }
                else if (cur_code == MATRIX_START_COLOR)
                    // first step of tail fading
                    m_fb->setPixel(x, y, rgb_from_code(MATRIX_DIM_COLOR));
                else
                    // otherwise just lower the brightness one step
                    m_fb->setPixel(x, y, rgb_from_code(cur_code - MATRIX_STEP));
            }

            // upper line processing
            rgb_t cur_color = m_fb->getPixel(x, m_fb->getHeight() - 1);
            uint32_t cur_code = rgb_to_code(cur_color);

            // if current top pixel is off, fill it with some probability
            if (cur_code == 0)
            {
                if (random8_to(m_density) == 0)
                    m_fb->setPixel(x, m_fb->getHeight() - 1, rgb_from_code(MATRIX_START_COLOR));
            }
            // if current pixel is almost off, try to make the fading out slower
            else if (cur_code <= MATRIX_ALMOST_OFF)
            {
                if (cur_code >= MATRIX_OFF_THRESH)
                    m_fb->setPixel(x, m_fb->getHeight() - 1, rgb_from_code(MATRIX_DIMMEST_COLOR));
                else
                    m_fb->setPixel(x, m_fb->getHeight() - 1, rgb_from_code(0));
            }
            else if (cur_code == MATRIX_START_COLOR)
                // first step of tail fading
                m_fb->setPixel(x, m_fb->getHeight() - 1, rgb_from_code(MATRIX_DIM_COLOR));
            else
                // otherwise just lower the brightness one step
                m_fb->setPixel(x, m_fb->getHeight() - 1, rgb_from_code(cur_code - MATRIX_STEP));
        }

        // Releases the lock/mutex
        return m_fb->endFrame();
    }

    ~Matrix() = default;

private:
    uint8_t m_density;
};
#endif