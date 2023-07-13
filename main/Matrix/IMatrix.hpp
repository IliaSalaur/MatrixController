#pragma once

#ifndef WS_IMATRIX_H
#define WS_IMATRIX_H

#include <cstdint>
#include <memory>

#include <Effects/Framebuffer.hpp>

static const uint8_t redraw_period = 17; //17 ms ~ 60 fps

/// @brief Interface - Abstract Class for interacting with the matrix
class IMatrix
{
protected:
    enum child_matrix_side_e{
        LEFT = 0,
        RIGHT,
        TOP,
        BOTTOM
    };

    const uint8_t _width;
    const uint8_t _height;
    uint8_t _totalWidth;
    uint8_t _totalHeight;
    uint8_t _leftSideWidth;
    uint8_t _rightSideWidth;

    std::unique_ptr<IMatrix> _leftChild;
    std::unique_ptr<IMatrix> _rightChild;   

    uint8_t _processRightWidth();
    uint8_t _processLeftWidth();
    void _processTotalWidth();
    void _processTotalHeight();
    void _redrawAll();

    /// @brief Method for redrawing the matrix
    virtual void _redraw() = 0;

    // /// @brief Method for drawing pixels. Must be implemented
    // /// @param x processed x coordinate of the local matrix
    // /// @param y processed y coordinate of the local matrix
    // /// @param col hex rgb24 color 
    virtual void _drawPixel(int x, int y, rgb_t col) = 0;
public:
    /// @param width matrix width
    /// @param height matrix height
    IMatrix(uint8_t width, uint8_t height); 

    virtual void begin() = 0;

    virtual void setBrightness(uint8_t brig);
    
    ///  @brief Method for manipulating one pixel at time
    ///  @param x x coordinate of the matrix. If negative, handled to the left child matrix
    ///  @param y y coordinate of the matrix. If negative, handled to the left child matrix
    ///  @param col hex rgb24 color
    void showPixel(int x, int y, rgb_t col);

    ///  @brief Method for manipulating multiple pixels at ones. 
    ///  @param leds pointer to the leds 2d array. Colors are arranged in one dimension and parsed as 2 dimensions
    ///  @param size size of the array
    void show(const std::vector<std::vector<rgb_t>>& leds, size_t size);

    ///  @brief Method for manipulating multiple pixels at ones. 
    ///  @param framebuffer framebuffer object, that contains all pixel data
    void show(Framebuffer& framebuffer);

    ///  @brief Method for connecting additional matrices on the left side of the main matrix
    ///  @param leftChild IMatrix* pointer to the left child matrix. Handled by an unique_ptr
    void addLeftChild(IMatrix* leftChild);

    ///  @brief Method for connecting additional matrices on the right side of the main matrix
    ///  @param rightChild IMatrix* pointer to the right child matrix. Handled by an unique_ptr
    void addRightChild(IMatrix* rightChild);

    /// @brief  Method for getting the total width of the matrices
    /// @return uint8_t total width 
    uint8_t getTotalWidth();

    /// @brief  Method for getting the total height of the matrices
    /// @return uint8_t total height 
    uint8_t getTotalHeight();    

    /// @brief Method for handling the matrix. Transmits the rgb_t buffer to the physical matrix when calling
    void handle();

    /// @brief Renders the frame and displays
    /// @param fb reference to the framebuffer
    /// @return ESP_OK on success 
    esp_err_t renderer(Framebuffer& fb);

    virtual ~IMatrix();
};

#endif