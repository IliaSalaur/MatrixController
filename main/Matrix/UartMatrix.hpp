#ifndef MY_UART_MATRIX_HPP
#define MY_UART_MATRIX_HPP

#include "IMatrix.hpp"
#include <vector>

extern "C" 
{
    #include <driver/uart.h>
};

class UartMatrix : public IMatrix
{
private:
    uint8_t m_brig;
    uart_port_t m_port;
    int m_baudrate;

    std::vector<std::vector<rgb_t>> m_pixels;
     
    void _redraw() override;
    uint16_t _xyToIndex(uint8_t x, uint8_t y);
    void _drawPixel(int x, int y, rgb_t col) override;
public:
    UartMatrix(uint8_t w, uint8_t h, uart_port_t uartPort, int baudrate);
    void begin() override;
    void setBrightness(uint8_t brig) override;
};

#endif