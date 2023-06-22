#ifndef MY_FIRE_HPP
#define MY_FIRE_HPP

#include <cstdlib>

static constexpr rgb_t C_BLACK  = { 0, 0, 0};
static constexpr rgb_t C_WHITE  = { .r = 255, .g = 255, .b = 255 };
static constexpr rgb_t C_DBLUE  = { .r = 0,   .g = 0,   .b = 100 };
static constexpr rgb_t C_CYAN   = { .r = 0,   .g = 200, .b = 255 };
static constexpr rgb_t C_RED    = { .r = 255, .g = 0,   .b = 0 };
static constexpr rgb_t C_YELLOW = { .r = 255, .g = 255, .b = 0 };
static constexpr rgb_t C_DGREEN = { .r = 0,   .g = 100, .b = 0 };
static constexpr rgb_t C_BGREEN = { .r = 155, .g = 255, .b = 155 };
static constexpr size_t k_fire_palette_size = 16;

extern "C"
{
    #include <lib8tion.h>
    #include <noise.h>
};

#include "AbstractEffect.hpp"

class Fire : public AbstractEffect
{
public:
    enum palette_e{
        PALETTE_FIRE = 0,
        PALETTE_BLUE,
        PALETTE_GREEN
    };

    Fire(Framebuffer* fb) : AbstractEffect{fb, EffectsEnum::FIRE}, m_palette{PALETTE_FIRE}, m_gradient{}
    {
        _fillGradient();
    }

    void setPropertiesFromMap(const effect_properties_t& props) override
    {
        if(const auto it = props.find("palette"); it != props.end())
        {
            this->setPalette(palette_e(std::stoul(it->second)));
        }
    } 

    palette_e getPalette()
    {
        return m_palette;
    }

    void setPalette(palette_e palette)
    {
        m_palette = palette;
        _fillGradient();
    }

    esp_err_t generateFrame() override
    {
        // Sets a lock/mutex
        m_fb->beginFrame();

        uint32_t a = esp_timer_get_time() / 1000;
        for (size_t x = 0; x < m_fb->getWidth(); x++)
            for (size_t y = 0; y < m_fb->getHeight(); y++)
            {
                uint8_t idx = qsub8(inoise8_3d(x * 60, y * 60 + a, a / 3), abs8(y - (m_fb->getHeight() - 1)) * 255 / (m_fb->getHeight() - 1));
                rgb_t c = color_from_palette_rgb(m_gradient, k_fire_palette_size, idx, 255, true);
                m_fb->setPixel(x, m_fb->getHeight() - y - 1, c);
            }

        // Releases the lock/mutex
        return m_fb->endFrame();
    }

    ~Fire() = default;

private:
    palette_e m_palette;
    rgb_t m_gradient[k_fire_palette_size];

    void _fillGradient()
    {
        switch (m_palette)
        {
            case PALETTE_BLUE:
                rgb_fill_gradient4_rgb(m_gradient, k_fire_palette_size, C_BLACK, C_DBLUE, C_CYAN, C_WHITE);
                break;
            case PALETTE_GREEN:
                rgb_fill_gradient4_rgb(m_gradient, k_fire_palette_size, C_BLACK, C_DGREEN, C_BGREEN, C_WHITE);
                break;
            default:
                rgb_fill_gradient4_rgb(m_gradient, k_fire_palette_size, C_BLACK, C_RED, C_YELLOW, C_WHITE);
        }
    }
};

#endif