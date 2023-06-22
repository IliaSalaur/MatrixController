#include "led_strip.h"
#include "esp_log.h"
#include "esp_err.h"
#include "sdkconfig.h"

#include "WS2812Matrix.hpp"

// 10MHz resolution, 1 tick = 0.1us (led strip needs a high resolution)
constexpr uint32_t k_led_strip_rmt_res_hz = (10 * 1000 * 1000);

WS2812Matrix::WS2812Matrix(uint8_t w, uint8_t h, gpio_num_t pin, rmt_channel_t rmtChannel)
    : 
    IMatrix{w, h},
    m_brig{100},
    m_wsMatrix{
        LED_STRIP_WS2812,
        false,
        m_brig,
        (size_t)w * h,
        pin,
        rmtChannel,
        NULL
    }
{
}

uint16_t WS2812Matrix::_xyToIndex(uint8_t x, uint8_t y)
{    
    return ((y % 2 == 0) ? y * _width + _width - x - 1:(y * _width) + x);
}

void WS2812Matrix::_redraw()
{
    for(uint16_t x = 0; x < _width; x++)
    {
        for(uint16_t y = 0; y < _height; y++)
        {
            ESP_ERROR_CHECK(led_strip_set_pixel(
                &this->m_wsMatrix,
                this->_xyToIndex(x, y),
                _pixels[x][y]
            ));
        }
    }

    ESP_ERROR_CHECK(led_strip_flush(&this->m_wsMatrix));
}

void WS2812Matrix::begin()
{
    // Install the LED Strip. Must be called before any led_strip methods
    led_strip_install();

    ESP_ERROR_CHECK(led_strip_init(&this->m_wsMatrix));
    
}

void WS2812Matrix::setBrightness(uint8_t brig)
{
    this->m_wsMatrix.brightness = brig;
}

// ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, 0, 16, 16, 16));
//             /* Refresh the strip to send data */
//             ESP_ERROR_CHECK(led_strip_refresh(led_strip));
//         } else {
//             /* Set all LED off to clear all pixels */
//             ESP_ERROR_CHECK(led_strip_clear(led_strip));