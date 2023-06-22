#ifndef MY_FRAMEBUFFER_HPP
#define MY_FRAMEBUFFER_HPP

#include <functional>

extern "C"
{
    #include <framebuffer.h>
};

/**
 * @brief A C++ wrapper for a framebuffer
 * 
 */
class Framebuffer
{
private:
    framebuffer_t m_fb; 
    std::function<esp_err_t(Framebuffer& fb)> m_rendererCallback;

    static Framebuffer* pCurrentBuffer;
    static esp_err_t _staticRendererWrapper(framebuffer_t* fb, void* arg);    

    framebuffer_t* _getFramebuffer();
public:
    /**
     * @brief Construct a new Framebuffer object
     * 
     * @param width width of the framebuffer
     * @param height height of the framebuffer
     * @param render_frame renderer, called upon a certain interval
     */
    Framebuffer(size_t width, size_t height, std::function<esp_err_t(Framebuffer& fb)> renderer);

    /**
     * @brief Method for resizing the framebuffer
     * 
     * @param width 
     * @param height 
     */
    void resize(size_t width, size_t height);  

    /**
     * @brief Call this at the begining of the effect's generateFrame() function. 
     * 
     */
    void beginFrame();

    /**
     * @brief Sets all pixels of the framebuffer to 0
     * 
     */
    void clear();

    /**
     * @brief Get the Width of the framebuffer
     * 
     * @return size_t
     */
    size_t getWidth() const;

    /**
     * @brief Get the Height of the framebuffer
     * 
     * @return size_t 
     */
    size_t getHeight() const;

    /**
     * @brief Get the Frame Number of the framebuffer
     * 
     * @return size_t 
     */
    size_t getFrameNumber();

    rgb_t getPixel(size_t x, size_t y);

    /**
     * @brief Set the rgb color of a pixel
     * 
     * @param x x coordinate
     * @param y y coordinate
     * @param color rgb representation of the color
     */
    void setPixel(size_t x, size_t y, rgb_t color);

    /**
     * @brief Set the rgb color of a pixel
     * 
     * @param x x coordinate
     * @param y y coordinate
     * @param color hex-rgb representation of the color
     */
    void setPixel(size_t x, size_t y, uint32_t color);

    /**
     * @brief Set the hsv color of a pixel
     * 
     * @param x x coordinate
     * @param y y coordinate
     * @param color hsv representation of the color
     */
    void setPixel(size_t x, size_t y, hsv_t color);

    /**
     * @brief Fades the framebuffer
     * 
     * @param scale Scale of fading
     */
    void fade(uint8_t scale);

    /**
     * @brief Blur the framebuffer in 2d
     * 
     * @param amount Amount of blur bassed as a fraction
     */
    void blur2d(fract8 amount);

    /**
     * @brief Call this at the end of the effect's generateFrame() function
     * 
     * @return esp_err_t 
     */
    esp_err_t endFrame();

    /**
     * @brief Destroy the Framebuffer object, free the internal framebuffer_t
     * 
     */
    ~Framebuffer();

    friend class Animation;
};
#endif