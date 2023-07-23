#include "Framebuffer.hpp"

extern "C"
{
    #include "esp_log.h"
    esp_err_t fb_init(framebuffer_t *fb, size_t width, size_t height, fb_render_cb_t render_cb);
    esp_err_t fb_free(framebuffer_t *fb);
    esp_err_t fb_set_pixel_rgb(framebuffer_t *fb, size_t x, size_t y, rgb_t color);
    esp_err_t fb_set_pixel_hsv(framebuffer_t *fb, size_t x, size_t y, hsv_t color);
    esp_err_t fb_get_pixel_rgb(framebuffer_t *fb, size_t x, size_t y, rgb_t *color);
    esp_err_t fb_begin(framebuffer_t *fb);
    esp_err_t fb_end(framebuffer_t *fb);
};

Framebuffer *Framebuffer::pCurrentBuffer = nullptr;

esp_err_t Framebuffer::_staticRendererWrapper(framebuffer_t *fb, void *arg)
{
    if (pCurrentBuffer == nullptr)
    {
        ESP_LOGE("framebuffer", "pCurrentBuffer is null");
        return ESP_ERR_NOT_FINISHED;
    }        

    if (pCurrentBuffer->m_rendererCallback)
        return pCurrentBuffer->m_rendererCallback(*pCurrentBuffer);

    return ESP_ERR_INVALID_ARG;
}

framebuffer_t *Framebuffer::_getFramebuffer()
{
    return &m_fb;
};

Framebuffer::Framebuffer(size_t width, size_t height, std::function<esp_err_t(Framebuffer &fb)> renderer) : m_fb{}, m_rendererCallback{renderer}
{
    // pCurrentBuffer = this;
    ESP_ERROR_CHECK(fb_init(
        &m_fb,
        width,
        height,
        Framebuffer::_staticRendererWrapper));
}

void Framebuffer::resize(size_t width, size_t height)
{
    if (width == m_fb.width && height == m_fb.height)
        return;
    // Callback<esp_err_t(framebuffer_t* fb, void* arg)>::func = std::bind(&Framebuffer::_rendererWrapper, this, std::placeholders::_1, std::placeholders::_2);
    // esp_err_t(*c_func)(framebuffer_t* fb, void* arg) = static_cast<decltype(c_func)>(Callback<esp_err_t(framebuffer_t* fb, void* arg)>::callback);
    ESP_ERROR_CHECK(fb_free(&m_fb));
    ESP_ERROR_CHECK(fb_init(
        &m_fb,
        width,
        height,
        Framebuffer::_staticRendererWrapper));
}

void Framebuffer::beginFrame()
{    
    ESP_ERROR_CHECK(fb_begin(&m_fb));
    pCurrentBuffer = nullptr;
}

void Framebuffer::clear()
{
    ESP_ERROR_CHECK(fb_clear(&m_fb));
}

size_t Framebuffer::getWidth() const
{
    return m_fb.width;
}

size_t Framebuffer::getHeight() const
{
    return m_fb.height;
}

size_t Framebuffer::getFrameNumber()
{
    return m_fb.frame_num;
}

rgb_t Framebuffer::getPixel(size_t x, size_t y)
{
    rgb_t color{};
    ESP_ERROR_CHECK(fb_get_pixel_rgb(&this->m_fb, x, y, &color));
    return color;
}

uint32_t Framebuffer::getHEX(size_t x, size_t y)
{
    rgb_t color{};
    ESP_ERROR_CHECK(fb_get_pixel_rgb(&this->m_fb, x, y, &color));
    return rgb_to_code(color);
}

void Framebuffer::setPixel(size_t x, size_t y, rgb_t color)
{
    if (x >= m_fb.width || y >= m_fb.height)
        return;
    fb_set_pixel_rgb(&m_fb, m_fb.width - x - 1, y, color);
}

void Framebuffer::setPixel(size_t x, size_t y, uint32_t color)
{
    if (x >= m_fb.width || y >= m_fb.height)
        return;
    fb_set_pixel_rgb(&m_fb, m_fb.width - x - 1, y, rgb_from_code(color));
}

void Framebuffer::setPixel(size_t x, size_t y, hsv_t color)
{
    if (x >= m_fb.width || y >= m_fb.height)
        return;
    fb_set_pixel_hsv(&m_fb, m_fb.width - x - 1, y, color);
}

void Framebuffer::fade(uint8_t scale)
{
    ESP_ERROR_CHECK(fb_fade(&m_fb, scale));
}

void Framebuffer::blur2d(fract8 amount)
{
    ESP_ERROR_CHECK(fb_blur2d(&m_fb, amount));
}

esp_err_t Framebuffer::endFrame()
{
    pCurrentBuffer = this;
    return fb_end(&m_fb);
}

void Framebuffer::copyIntoFramebuffer(const void *dataPtr, size_t size)
{
    memcpy(
        m_fb.data, 
        dataPtr, 
        std::min(size, m_fb.width * m_fb.height * (size_t)sizeof(rgb_t)));
}

Framebuffer::~Framebuffer()
{
    ESP_ERROR_CHECK(fb_free(&m_fb));
}