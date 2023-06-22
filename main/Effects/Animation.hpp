#ifndef MY_ANIMATION_HPP
#define MY_ANIMATION_HPP

#include <functional>

#include <framebuffer.h>
#include <fbanimation.h>
#include "AbstractEffect.hpp"
#include "Framebuffer.hpp"
#include <FunctionCallbackHelper.hpp>

class Animation
{
private:
    size_t m_fps;
    Framebuffer* m_fb;
    fb_animation_t m_animation;
    AbstractEffect* m_effect;
    bool m_isPlaying;

    /**
     * @brief Wrapper for the AbstractEffect's generateFrame function,  
     * 
     * @param fb pointer to the framebuffer_t (AbstractEffect's framebuffer used instead)
     * @return esp_err_t result of generateFrame()
     */
    esp_err_t _generateFrameWrapper(framebuffer_t* fb)
    {
        if(m_effect)    return m_effect->generateFrame();
        return ESP_OK;
    }

public:
    Animation(Framebuffer* fb, size_t fps) : m_fps{fps}, m_fb{fb}, m_animation{}, m_effect{nullptr}, m_isPlaying{false}
    {
        fb_animation_init(&m_animation, fb->_getFramebuffer());
    }

    void setFPS(size_t fps)
    {
        m_fps = fps > 0 ? fps : m_fps;
    }

    void stop()
    {
        // Clear the pointer
        m_effect = nullptr;

        // Stop the animation, if it is playing
        if(m_isPlaying == true)    ESP_ERROR_CHECK(fb_animation_stop(&m_animation));

        m_isPlaying = false;
    }

    void play(AbstractEffect* effect, void* ctx = nullptr)
    {
        // Stop the last animation, is it is playing
        if(m_isPlaying == true)     this->stop();

        // Set the pointer to the effect
        m_effect = effect;

        // Bind the _generateFrameWrapper to the func, a std::function<>
        Callback<esp_err_t(framebuffer_t* fb)>::func = std::bind(&Animation::_generateFrameWrapper, this, std::placeholders::_1);
        // Convert the std::function into C function by calling it through a wrapper
        esp_err_t(*c_func)(framebuffer_t* fb) = static_cast<decltype(c_func)>(Callback<esp_err_t(framebuffer_t* fb)>::callback);

        // Run the animation
        ESP_ERROR_CHECK(fb_animation_play(&m_animation, m_fps, c_func, ctx));

        m_isPlaying = true;
    }

    bool isPlaying()
    {
        return m_isPlaying;
    }

    ~Animation()
    {
        fb_animation_free(&m_animation);
    }
};

#endif