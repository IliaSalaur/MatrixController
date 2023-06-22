#include "UartMatrix.hpp"
extern "C"
{
    #include <esp_log.h>
    #include <esp_err.h>
};

UartMatrix::UartMatrix(uint8_t w, uint8_t h, uart_port_t uartPort, int baudrate)
    :
    IMatrix{w, h},
    m_brig{100},
    m_port{uartPort},
    m_baudrate{baudrate}
{

}

void UartMatrix::begin()
{
    uart_config_t uart_config{
        m_baudrate,                     // baudrate
        UART_DATA_8_BITS,               // data bits
        UART_PARITY_DISABLE,            // parity
        UART_STOP_BITS_1,               // stop bits               
        UART_HW_FLOWCTRL_DISABLE,       // flow control
        122,                            // rx flow control thresh
        UART_SCLK_DEFAULT,              // source clock
    };

    ESP_ERROR_CHECK(uart_driver_install(m_port, 256, 0, 0, NULL, 0));
    // Set the config
    ESP_ERROR_CHECK(uart_param_config(m_port, &uart_config));

    // Configure pins: TX:4, RX:5, RTC:no change, CTS:no change
    ESP_ERROR_CHECK(uart_set_pin(m_port, 4, 5, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
}

void UartMatrix::setBrightness(uint8_t brig)
{

}

uint16_t UartMatrix::_xyToIndex(uint8_t x, uint8_t y)
{    
    return ((y % 2 == 0) ? y * _width + _width - x - 1:(y * _width) + x);
}

void UartMatrix::_redraw()
{
    const size_t dataLen = _width * _height * 3 + 1;
    uint8_t data[dataLen]{};
    size_t i = 0;
    for(uint16_t x = 0; x < _width; x++)
    {
        for(uint16_t y = 0; y < _height; y++)
        {
            rgb_t col = _pixels[x][y];

            data[i++] = col.r / 2 != '\r' ? col.r / 2 : 14;
            data[i++] = col.g / 2 != '\r' ? col.g / 2 : 14;
            data[i++] = col.b / 2 != '\r' ? col.b / 2 : 14;
       
            // uint8_t buf = (col.r & 0b11100000);
            // buf |= (col.g & 0b11000000) >> 3;
            // buf |= (col.b & 0b11100000) >> 5;            
            // data[i++] = buf == '\r' ? buf + 1 : buf;            
        }        
    }
    data[i++] = '\r';

    /* TX Buffer size is set to zero. 
        Because of that, uart_write_bytes will block the code execution
        until the last portion of the data is passed to the internal FIFO buffer
    */
    int64_t t = esp_timer_get_time();
    uart_write_bytes(m_port, data, i);
    // ESP_LOGI("uart", "uart_write_bytes done in %lldus", esp_timer_get_time() - t);
}