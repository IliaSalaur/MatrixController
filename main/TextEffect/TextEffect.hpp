#ifndef MY_TEXT_EFFECT_HPP
#define MY_TEXT_EFFECT_HPP

#include <string>
#include <memory>

#include "Fonts.h"
#include "Effects/AbstractEffect.hpp"
#include "Effects/EffectFactory.hpp"

#include "nlohmann/json.hpp"
using nlohmann::json;

static const char* TT_JS_TAG = "textTemplate_json";

struct TextTemplate
{    
    uint32_t letterCol;
    uint32_t backCol;
    size_t displayTime;
    size_t scrollTimes;
    EffectsEnum textFilter;    
    std::string text;
    effect_properties_t textFilterProps;

    TextTemplate() : letterCol{0}, backCol{0}, displayTime{0}, scrollTimes{0}, textFilter{EffectsEnum::NONE}, text{}, textFilterProps{}
    {

    }
    
    TextTemplate(uint32_t letterCol_, uint32_t backCol_, size_t displayTime_, size_t scrollTimes_, EffectsEnum textFilter_, std::string text_, effect_properties_t textFilterProperties_)
        :
        letterCol{letterCol_},
        backCol{backCol_},
        displayTime{displayTime_},
        scrollTimes{scrollTimes_},
        textFilter{textFilter_},    
        text{std::move(text_)},
        textFilterProps{std::move(textFilterProperties_)}
    {}

    // NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(TextTemplate, letterCol, backCol, displayTime, scrollTimes, textFilter, text, textFilterProps)
    friend void to_json(nlohmann::json& nlohmann_json_j, const TextTemplate& nlohmann_json_t)
    {
        nlohmann_json_j["letterCol"] = nlohmann_json_t;
        nlohmann_json_j["backCol"] = nlohmann_json_t;
        nlohmann_json_j["displayTime"] = nlohmann_json_t;
        nlohmann_json_j["scrollTimes"] = nlohmann_json_t;
        nlohmann_json_j["text"] = nlohmann_json_t;
        nlohmann_json_j["textFilter"] = nlohmann_json_t;
        nlohmann_json_j["textFilterProps"] = nlohmann_json_t.textFilterProps;
    }

    friend void from_json(const nlohmann::json& nlohmann_json_j, TextTemplate& nlohmann_json_t)
    {
        nlohmann_json_t.text = nlohmann_json_j.value("text", std::string{});
        ESP_LOGI(TT_JS_TAG, "text: %s", nlohmann_json_t.text.c_str());

        nlohmann_json_t.letterCol = std::stoul(nlohmann_json_j.value("letterCol", std::string{"#0"}).substr(1), nullptr, 16);
        nlohmann_json_t.backCol = std::stoul(nlohmann_json_j.value("backCol", std::string{"#0"}).substr(1), nullptr, 16);

        ESP_LOGI(TT_JS_TAG, "cols: %lu and %lu", nlohmann_json_t.letterCol, nlohmann_json_t.backCol);
        nlohmann_json_t.displayTime = nlohmann_json_j.value("displayTime", size_t{0});
        nlohmann_json_t.scrollTimes = (size_t)nlohmann_json_j.value("scrollTimes", size_t{0});
        nlohmann_json_t.textFilter = EffectsEnum{nlohmann_json_j.value("textFilter", 0)};

        // nlohmann_json_t.displayTime = static_cast<size_t>(nlohmann_json_j.value("displayTime", int{0}));
        // nlohmann_json_t.scrollTimes = static_cast<size_t>((size_t)nlohmann_json_j.value("scrollTimes", int{0}));
        // nlohmann_json_t.textFilter = EffectsEnum::NONE;
        // nlohmann_json_t.textFilter = EffectsEnum{nlohmann_json_j.value("textFilter", 0)};
        // nlohmann_json_t.textFilter = EffectsEnum{std::stoi(nlohmann_json_j.value("textFilter", "0"))};

        nlohmann_json_t.textFilterProps = effect_properties_t{};
        nlohmann_json_t.textFilterProps = nlohmann_json_j.value("textFilterProps", effect_properties_t{});
        ESP_LOGI(TT_JS_TAG, "done");
    }
};

class TextEffect : public AbstractEffect
{
private:
    TextTemplate m_template;
    uint8_t m_speed;
    Font5x7 m_font5x7;
    Font5x7Rus m_font5x7Rus;
    int m_x;
    int m_y; 
    size_t m_scrollsLeft;

    std::unique_ptr<AbstractEffect> m_filter;
    Framebuffer m_filterFb;

    esp_err_t _filterRenderer(Framebuffer& fb)
    {
        ESP_LOGI("text_effect", "Filter renderer called");
        return ESP_OK;
    }   

    /**
     * @brief Checks if the given point is within the matrix
     * 
     * @param x x coordinate
     * @param y y coordinate
     * @return true if the given coordinate is whithin the matrix width and height
     * @return false otherwise
     */
    bool _isInBounds(int x, int y)
    {
        return x < m_fb->getWidth() && x >= 0 && y < m_fb->getHeight() && y >= 0;
    }

    /**
     * @brief Draws a letter at the given coordinate with the given colors
     * 
     * @param x x coordinate
     * @param y y coordinate
     * @param letCol Color of the letter. The function will affect only the coordinates, that are within the letter's outline
     * @param letter char, if it is a multi char, ignores the first part and treats the char after as a cyrillyc one
     * @return true if the letter is succesfully displayed
     * @return false if the last char was a multipart one and the method should be called again
     */
    bool drawLetter(int x, uint8_t y, uint32_t letCol, char letter)
    {            
        static bool lastRus = false;
        static char lastChar = 0;
        // char buf[3] = {lastChar, letter, 0};
        // Fm("index: %d, char:%s\n", !lastRus ? letter : (int)(int(lastChar) << 8) | int(letter), buf)
        if(letter > 200)
        {
            lastRus = true;
            lastChar = letter;
            return false;
        }

        const uint8_t* fontBytes = lastRus ? m_font5x7Rus.getBytes((int)(int(lastChar) << 8) | int(letter)) : m_font5x7.getBytes(int(letter));
        for(uint8_t lx = 0; lx < 5; lx++)
        {            
            for(uint8_t ly = 0; ly < 7; ly++)
            {   if(_isInBounds(lx + x, ly + y) && fontBytes[lx] & (1 << (m_font5x7.getCharHeight() - ly - 1)))
                m_fb->setPixel(
                    lx + x,
                    ly + y,
                    (m_template.letterCol == 0xff000000 ? (m_filter ? m_filterFb.getPixel(lx + x, ly + y) : rgb_t{0, 0, 0}) : rgb_from_code(letCol))
                    // (m_template.letterCol == 0xff000000 ? rgb_t{0, 0, 0} : rgb_from_code(letCol))
                );
            }
        }
        lastRus = false;
        return true;
    }

    /**
     * @brief Handles text displaying routine
     * 
     */
    void _showText()
    {     
        //Fill the background
        for(size_t x = 0; x < m_fb->getWidth(); x++)
        {
            for(size_t y = 0; y < m_fb->getHeight(); y++)
            {
                m_fb->setPixel(
                    x,
                    y,
                    m_template.backCol == 0xff000000 ? (m_filter ? m_filterFb.getPixel(x, y) : rgb_t{0, 0, 0}) : rgb_from_code(m_template.backCol)
                    // m_template.backCol == 0xff000000 ? rgb_t{0, 0, 0} : rgb_from_code(m_template.backCol)
                );
            }
        }

        // 1 - text spacing
        for(int x = 0, i = 0; i < m_template.text.length(); x += m_font5x7.getCharWidth() + 1, i++)
        {
            bool res = drawLetter(
                x + m_x,
                m_y,
                m_template.letterCol,
                m_template.text[i] & 0xff
            );

            if(!res)
            {
                drawLetter(
                    x + m_x,
                    m_y,
                    m_template.letterCol,
                    m_template.text[++i] & 0xff
                );
            }
        }
    }

    /**
     * @brief Get the actual count of chars to be displayed on the matrix
     * @param str a std::string
     * @return processed length of str
     */
    size_t _getActualLength(const std::string& str)
    {
        size_t l = 0;
        for(char c : str)
            l += int(c) < 207;
    
        return l;
    }

public:
    TextEffect(Framebuffer* fb, TextTemplate textTemplate)
        :
        AbstractEffect{fb, EffectsEnum::TEXT},
        m_template{textTemplate},
        m_speed{100},
        m_font5x7{},
        m_font5x7Rus{},
        m_x{(int)m_fb->getWidth()},
        m_y{((int)fb->getHeight() - m_font5x7.getCharHeight()) / 2},
        
        m_filter{nullptr},
        m_filterFb{
            fb->getWidth(), 
            fb->getHeight(), 
            std::bind(&TextEffect::_filterRenderer, this, std::placeholders::_1)
        }
    {
        
    }

    /**
     * @brief Sets the internal TextTemplate properties from a map
     * 
     * @param props effect_properties_t aka std::map<std::string, std::string>
     */
    void setPropertiesFromMap(const effect_properties_t& props) override
    {
        if(const auto it = props.find("text"); it != props.end())
        {            
            m_template.text = it->second;
            ESP_LOGI("text_effect", "Text set to %s", m_template.text.c_str());
        }

        if(const auto it = props.find("letterCol"); it != props.end())
        {
            m_template.letterCol = std::stoul(it->second, 0, 16);
            ESP_LOGI("text_effect", "letterCol set to %lu", m_template.letterCol);
        }

        if(const auto it = props.find("backCol"); it != props.end())
        {
            m_template.backCol = std::stoul(it->second, 0, 16);
            ESP_LOGI("text_effect", "backCol set to %lu", m_template.backCol);
        }

        if(const auto it = props.find("displayTime"); it != props.end())
        {
            m_template.displayTime = std::stoul(it->second);
            ESP_LOGI("text_effect", "displayTime set to %u", m_template.displayTime);
        }

        if(const auto it = props.find("scrollTimes"); it != props.end())
        {
            m_template.scrollTimes = std::stod(it->second);
            m_scrollsLeft = m_template.scrollTimes;
            ESP_LOGI("text_effect", "scrollTimes set to %d", m_template.scrollTimes);
        }

        // if(const auto it = props.find("textFilter"); it != props.end())
        // {
        //     m_template.textFilter = EffectsEnum(std::stoul(it->second));
        //     ESP_LOGI("text_effect", "textFilter set to %d", int(m_template.textFilter));
        // }
    } 

    void setTextTemplate(TextTemplate textTemplate)
    {
        m_template = textTemplate;
        m_scrollsLeft = textTemplate.scrollTimes;
        m_x = (int)m_fb->getWidth();
        this->setFilter(textTemplate.textFilter, textTemplate.textFilterProps);
    }

    void setSpeed(uint8_t speed)
    {
        m_speed = 255 - speed;
    }

    uint8_t getSpeed()
    {
        return m_speed;
    }

    void setFilter(const EffectsEnum& filterEnum, const effect_properties_t& props)
    {
        if(m_filter && filterEnum == m_filter->getEffect())
        {
            m_filter->setPropertiesFromMap(props);
            return;
        }

        m_filter = EffectFactory::getEffect(&m_filterFb, filterEnum, props);
    }

    size_t getSrollsLeft()
    {
        return m_scrollsLeft;
    }

    esp_err_t generateFrame() override
    {
        // #error Co-existence of 2 Framebuffers(.hpp) is imposible, because of the structure holding the c_func wrapper being static
        // Variable to count milliseconds
        static int64_t tmr = 0;        

        // Generate the frame of the effect, fill the m_filterFb
        // The generateFrame method can't be called inside a {beginFrame() --- endFrame()} block, due to the specifics of the framebuffer's rendering
        if(m_filter) ESP_ERROR_CHECK(m_filter->generateFrame());

        // Sets a lock/mutex
        m_fb->beginFrame();

        // Move the letter after a certain amount of time, defined as m_speed
        if(m_scrollsLeft && esp_timer_get_time() / 1000 - tmr >= m_speed)
        {            
            // Keep the current value of esp_timer_get_time() divided by a 1000, in order to obtain the uptime in ms
            tmr = esp_timer_get_time() / 1000;   

            // Move the text            
            --m_x;                   

            // Because of the UTF8 encoding, an actual character could spread up to 4 bytes.  
            const size_t textLength = this->_getActualLength(m_template.text);
            // Text width in pixels
            const int textWidthPx = textLength * m_font5x7.getCharWidth();

            ESP_LOGI("text_effect", "tmr is %lld; x is %d, textWidthPx is %d", tmr, m_x, textWidthPx);

            // Check if the text is in the middle. If so -> wait.
            if(textWidthPx < m_fb->getWidth() && m_x == (m_fb->getWidth() - textWidthPx) / 2)
            {
                tmr += m_template.displayTime * 1000;
            }
            
            // Check if the text has passed the border of the matrix. If it did, wait
            if(m_x < -(textWidthPx + (int)m_fb->getWidth()))
            {
                // Reset the m_x to the start position
                m_x = (int)m_fb->getWidth();

                --m_scrollsLeft;
                ESP_LOGI("text", "Text is %s, scrollsLeft: %u", m_template.text.c_str(), m_scrollsLeft);
                // Wait 3 iterations, 1 iteration == m_speed == one letter appearing
                tmr += m_speed * 3;
            }
        }

        // Draw the text        
        this->_showText();  

        // Releases the lock/mutex
        return m_fb->endFrame();
    }

    ~TextEffect() = default;
};

#endif