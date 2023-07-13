#ifndef WS2812MATRIX_H_
#define WS2812MATRIX_H_

#include "IMatrix.hpp"
#include "led_strip.h"

class WS2812Matrix : public IMatrix
{
private:
    bool m_redrawRequired;
    uint8_t m_brig;    
    led_strip_t m_wsMatrix; 

    void _redraw() override;
    uint16_t _xyToIndex(uint8_t x, uint8_t y);
    void _drawPixel(int x, int y, rgb_t col) override;
public:
    WS2812Matrix(uint8_t w, uint8_t h, gpio_num_t pin, rmt_channel_t rmtChannel = RMT_CHANNEL_0);
    void begin() override;
    void setBrightness(uint8_t brig) override;
    void clearDisplay();
};

#endif