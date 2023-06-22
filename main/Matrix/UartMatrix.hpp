#ifndef MY_UART_MATRIX_HPP
#define MY_UART_MATRIX_HPP

#include "IMatrix.hpp"

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
     
    void _redraw() override;
    uint16_t _xyToIndex(uint8_t x, uint8_t y);
public:
    UartMatrix(uint8_t w, uint8_t h, uart_port_t uartPort, int baudrate);
    void begin() override;
    void setBrightness(uint8_t brig) override;
};

#endif