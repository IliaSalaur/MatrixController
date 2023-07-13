#include "IMatrix.hpp"
#include "freertos/task.h"

IMatrix::IMatrix(uint8_t width, uint8_t height) : 
    _width(width),
    _height(height),
    _totalWidth(_width),
    _totalHeight(_height),
    _leftSideWidth(0),
    _rightSideWidth(0),
    _leftChild(nullptr),
    _rightChild(nullptr)
{
    
}

void IMatrix::setBrightness(uint8_t brig)
{
    
}

uint8_t IMatrix::_processRightWidth()
{
    _rightSideWidth = (_rightChild == nullptr) ? 0 : _rightChild->_width + _rightChild->_processRightWidth();
    return _rightSideWidth;
}

uint8_t IMatrix::_processLeftWidth()
{
    _leftSideWidth = (!_leftChild) ? 0 : _leftChild->_width + _leftChild->_processLeftWidth();
    return _leftSideWidth;
}


void IMatrix::showPixel(int x, int y, rgb_t col)
{
    if(x >= _width || y >= _height)
    {   if(!_rightChild) return;
        _rightChild->showPixel((x >= _width ? x - _width : x), (y >= _height ? y - _height : y), col);
        return;
    }
    else if(x < 0 || y < 0)
    {
        _leftChild->showPixel((x < 0 ? x + _width : x), (y < 0 ? y + _height : y), col);
        return;
    }   
    this->_drawPixel(x, y, col);
}

void IMatrix::show(const std::vector<std::vector<rgb_t>>& leds, size_t size)
{
    for(int x = -_leftSideWidth; x < _rightSideWidth + _width; x++)
    {
        for(int y = 0; y < _totalHeight; y++)
        {   
            this->showPixel(x, y, leds[x + _leftSideWidth][y]);
        }
    }

    this->_redrawAll();
}

void IMatrix::show(Framebuffer& framebuffer)
{
    for(int x = -_leftSideWidth; x < _rightSideWidth + _width; x++)
    {
        for(int y = 0; y < _totalHeight; y++)
        {   
            // this->showPixel(x, y, leds[x + _leftSideWidth][y]);
            this->showPixel(x, y, framebuffer.getPixel(x + _leftSideWidth, y));
        }
    }

    this->_redrawAll();
}

void IMatrix::addLeftChild(IMatrix* leftChild)
{
    if(_leftChild){
        _leftChild->addLeftChild(leftChild);
    } 
    else{
        _leftChild.reset(leftChild);
    }
    _processTotalHeight();
    _processTotalWidth();
}

void IMatrix::addRightChild(IMatrix* rightChild)
{
    if(_rightChild){
        _rightChild->addRightChild(rightChild);
    } 
    else{
        _rightChild.reset(rightChild);
    }
    _processTotalHeight();
    _processTotalWidth();
}

void IMatrix::_processTotalWidth()
{
    _totalWidth = this->_processRightWidth() + this->_processLeftWidth() + _width;
}

void IMatrix::_processTotalHeight()
{
    // _totalHeight = this->_processSideHeight(LEFT) + this->_processSideHeight(RIGHT);
    _totalHeight = _height;
}

uint8_t IMatrix::getTotalWidth()
{
    return _totalWidth;
}

uint8_t IMatrix::getTotalHeight()
{
    return _totalHeight;
}

void IMatrix::_redrawAll()
{
    this->_redraw();
    if(_leftChild) _leftChild->_redrawAll();
    if(_rightChild) _rightChild->_redrawAll();
}

void IMatrix::handle()
{
    this->_redrawAll();
}

esp_err_t IMatrix::renderer(Framebuffer &fb)
{
    this->show(fb);
    this->handle();


    return ESP_OK;
}

IMatrix::~IMatrix() = default;
