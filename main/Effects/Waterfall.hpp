#ifndef MY_WATERFALL_HPP
#define MY_WATERFALL_HPP

#include <cstdlib>
#include <vector>
#include <cstdint>

static constexpr size_t k_waterfall_palette_size = 16;
#define MAP_XY(x, y) ((y) * m_fb->getWidth() + (x))

extern "C"
{
    #include <lib8tion.h>
    #include <noise.h>
    #include "esp_log.h"
};

#include "AbstractEffect.hpp"

class Waterfall : public AbstractEffect
{
public:
    enum mode_e{
        WATERFALL_SIMPLE = 0,
        WATERFALL_COLORS,
        WATERFALL_FIRE,
        WATERFALL_COLD_FIRE,
    };

    Waterfall(Framebuffer* fb) : 
        AbstractEffect{fb, EffectsEnum::WATERFALL}, 
        m_mode{}, 
        m_hue{}, 
        m_cooling{}, 
        m_sparking{}, 
        m_palette{},
        m_map(m_fb ? m_fb->getWidth() * m_fb->getHeight() : 1, 0)        
    {

    }

    void setPropertiesFromMap(const effect_properties_t& props) override
    {
        bool redrawGradient = false;
        if(const auto it = props.find("hue"); it != props.end())
        {
            m_hue = std::stoul(it->second);
            redrawGradient = true;
        }

        if(const auto it = props.find("mode"); it != props.end())
        {
            m_mode = mode_e(std::stoul(it->second));
            redrawGradient = true;
        }

        if(const auto it = props.find("cooling"); it != props.end())
        {
            m_cooling = std::stoul(it->second);
        }

        if(const auto it = props.find("sparking"); it != props.end())
        {
            m_sparking = std::stoul(it->second);
        }

        if(redrawGradient) _generateGradient();

        ESP_LOGI(
            "waterfall", 
            "hue:%u, sparking:%u, cooling:%u, mode:%u, redrawed:%u",
            m_hue,
            m_sparking,
            m_cooling,
            uint8_t(m_mode),
            redrawGradient
        );
    } 

    void setFrameBuffer(Framebuffer* fb) override
    {
        m_fb = fb;
        m_map.resize(m_fb->getHeight() * m_fb->getWidth(), 0);
    }

    void setMode(mode_e mode)
    {
        m_mode = mode;
        _generateGradient();
    }

    mode_e getMode(){
        return m_mode;
    }

    void setHue(uint8_t hue)
    {
        m_hue = hue;
        _generateGradient();
    }

    uint8_t getHue()
    {
        return m_hue;
    }

    void setCooling(uint8_t cooling)
    {
        m_cooling  = cooling;
    }

    uint8_t getCooling()
    {
        return m_cooling;
    }

    void setSparking(uint8_t sparking)
    {
        m_sparking = sparking;
    }

    uint8_t getSparking()
    {
        return m_sparking;
    }
    

    esp_err_t generateFrame() override
    {
        // Sets a lock/mutex
        m_fb->beginFrame();

        for (size_t x = 0; x < m_fb->getWidth(); x++)
        {
            size_t y;

            // Step 1.  Cool down every cell a little
            for (y = 0; y < m_fb->getHeight(); y++)
                m_map[MAP_XY(x, y)] = qsub8(m_map[MAP_XY(x, y)], random8_to((m_cooling * 10 / m_fb->getHeight()) + 2));

            // Step 2.  Heat from each cell drifts 'up' and diffuses a little
            for (y = m_fb->getHeight() - 1; y >= 2; y--)
                m_map[MAP_XY(x, y)] =
                        (m_map[MAP_XY(x, y - 1)] + m_map[MAP_XY(x, y - 2)] + m_map[MAP_XY(x, y - 2)]) / 3;

            // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
            if (random8() < m_sparking)
            {
                y = random8_to(2);
                m_map[MAP_XY(x, y)] = qadd8(m_map[MAP_XY(x, y)], random8_between(160, 255));
            }

            // Step 4.  Map from heat cells to LED colors
            for (y = 0; y < m_fb->getHeight(); y++)
            {
                // Scale the heat value from 0-255 down to 0-240
                // for best results with color palettes.
                uint8_t color_idx = scale8(m_map[MAP_XY(x, y)], 240);
                bool is_fire = (m_mode == WATERFALL_FIRE || m_mode == WATERFALL_COLD_FIRE);
                m_fb->setPixel(x, is_fire ? y : m_fb->getHeight() - 1 - y,
                        color_from_palette_rgb(m_palette, k_waterfall_palette_size, color_idx, 255, true));
            }
        }

        // Releases the lock/mutex
        return m_fb->endFrame();
    }

    ~Waterfall() = default;

private:
    mode_e m_mode;
    uint8_t m_hue;
    uint8_t m_cooling;
    uint8_t m_sparking;
    rgb_t m_palette[k_waterfall_palette_size];
    std::vector<uint8_t> m_map;

    void _generateGradient()
    {
        switch (m_mode)
        {
        case WATERFALL_SIMPLE:
            rgb_fill_gradient4_hsv(m_palette, k_waterfall_palette_size,
                    hsv_from_values(0, 0, 0),
                    hsv_from_values(m_hue, 0, 255),
                    hsv_from_values(m_hue, 128, 255),
                    hsv_from_values(m_hue, 255, 255),
                    COLOR_SHORTEST_HUES);
            break;
        case WATERFALL_COLORS:
            rgb_fill_gradient4_hsv(m_palette, k_waterfall_palette_size,
                    hsv_from_values(0, 0, 0),
                    hsv_from_values(m_hue, 0, 255),
                    hsv_from_values(m_hue, 128, 255),
                    hsv_from_values(255, 255, 255),
                    COLOR_SHORTEST_HUES);
            break;
        case WATERFALL_FIRE:
            rgb_fill_gradient4_rgb(m_palette, k_waterfall_palette_size,
                    rgb_from_values(0, 0, 0),       // black
                    rgb_from_values(255, 0, 0),
                    rgb_from_values(255, 255, 0),
                    rgb_from_values(255, 255, 255)); // white
            break;
        case WATERFALL_COLD_FIRE:
            rgb_fill_gradient4_rgb(m_palette, k_waterfall_palette_size,
                    rgb_from_values(0, 0, 0),       // black
                    rgb_from_values(0, 0, 100),
                    rgb_from_values(0, 200, 255),
                    rgb_from_values(255, 255, 255)); // white
            break;
        }
    }
};

#endif