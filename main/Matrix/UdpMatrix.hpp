#ifndef UDP_MATRIX_HPP_
#define UDP_MATRIX_HPP_

#include "IMatrix.hpp"
#include <string>
#include <vector>
#include "wifi_app/MatrixInfo.hpp"

class UdpMatrix : public IMatrix
{
private:
    bool m_redrawRequired;
    uint8_t m_brig;    
    TaskHandle_t m_taskHandle;
    std::vector<rgb_t> m_pixels;
    MatrixInfo m_childMatrixInfo;


    static void _taskWrapper(void* pvParameter);
    size_t _xyToIndex(uint8_t x, uint8_t y);
    void _redraw() override;
    void _drawPixel(int x, int y, rgb_t col) override;
public:
    UdpMatrix(uint8_t w, uint8_t h, MatrixInfo matrixInfo);
    void begin() override;
    void setBrightness(uint8_t brig) override;
};

#endif