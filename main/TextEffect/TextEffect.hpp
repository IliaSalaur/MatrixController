#ifndef MY_TEXT_EFFECT_HPP
#define MY_TEXT_EFFECT_HPP

#include <string>
#include <memory>

#include "Effects/EffectFactory.hpp"

#include "TextTemplate.hpp"

#define TEXT_EF_LOG(x...) 

class TextEffect : public AbstractEffect
{
private:
    TextTemplate m_template;
    uint8_t m_speed;
    
    int m_x;
    int m_y; 
    size_t m_scrollsLeft;

    std::unique_ptr<AbstractEffect> m_filter;
    Framebuffer m_filterFb;

    esp_err_t _filterRenderer(Framebuffer& fb)
    {
        TEXT_EF_LOG("text_effect", "Filter renderer called");
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
     * @brief Draws a glyph
     * 
     * @param xPos x coordinate
     * @param yPos y coordinate
     * @param letCol Color of the letter. The function will affect only the coordinates, that are within the letter's outline
     * @param glyph 
     * @param font 
     */
    void drawGlyph(int xPos, int yPos, uint32_t letCol, const GFXglyph& glyph, const GFXfont* font)
    {
        const uint8_t* bitmapPtr = font->bitmap + glyph.bitmapOffset;
        for(size_t bit = 0, x = 0, y = 0; bit < glyph.height * glyph.width; bit++, x = bit % glyph.width, y += (bit + 1) % glyph.width == 0)
        {
            // printf("%s%s",
            // bitmapPtr[bit / 8] & (128 >> (bit % 8)) ? "#" : " ",
            // (bit + 1) % glyph.width ? "" : "\n");       

            // check if in bounds and check if need to paint the pixel
            if(_isInBounds(xPos + x, yPos + y) && (bitmapPtr[bit / 8] & (128 >> (bit % 8))))
                m_fb->setPixel(
                    xPos + x,
                    yPos + (glyph.height - y),
                    (m_template.letterCol == 0xff000000 ? (m_filter ? m_filterFb.getPixel(xPos + x, yPos + y) : rgb_t{0, 0, 0}) : rgb_from_code(letCol))
                    // (m_template.letterCol == 0xff000000 ? rgb_t{0, 0, 0} : rgb_from_code(letCol))
                );
        }
    }

    /**
     * @brief Draws a letter at the given coordinate with the given colors. Checks whenever the letter is cyrillic, gets the coresponding glyph 
     * @param x x coordinate
     * @param y y coordinate
     * @param letter char, if it is a multi char, ignores the first part and treats the char after as a cyrillyc one
     * @retval true if the letter is succesfully displayed
     * @retval false if the letter isn't valid
     */
    // bool drawLetter(int x, uint8_t y, char letter)
    // {         
    //     static bool lastRus = false;
    //     static char lastChar = 0;
    //     // char buf[3] = {lastChar, letter, 0};
    //     // Fm("index: %d, char:%s\n", !lastRus ? letter : (int)(int(lastChar) << 8) | int(letter), buf)
    //     if(letter > 200)
    //     {
    //         lastRus = true;
    //         lastChar = letter;
    //         return false;
    //     }        

    //     if(lastRus)
    //     {
    //         letter = 
    //             (lastChar == 208 && letter == 129) ? 192 :                  // recoding 'Ë' letter
    //             ((lastChar == 209 && letter == 145) ? 193 : letter);        // recoding 'ё' letter
    //     }

    //     drawGlyph(m_x, m_y, m_template.letterCol, m_template.font->glyph[letter], m_template.font);

    //     lastRus = false;
    //     return true;
    // }

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
        // for(int x = 0, i = 0; i < m_template.text.length(); x += m_template.font->glyph, i++)
        // {
        //     // bool res = drawLetter(
        //     //     x + m_x,
        //     //     m_y,
        //     //     m_template.letterCol,
        //     //     m_template.text[i] & 0xff
        //     // );

        //     // if(!res)
        //     // {
        //     //     drawLetter(
        //     //         x + m_x,
        //     //         m_y,
        //     //         m_template.letterCol,
        //     //         m_template.text[++i] & 0xff
        //     //     );
        //     // }

        // }

        int x = 0;
        char lastC = 0;
        for(char c : m_template.text)
        {
            if(c > 200)
            {
                lastC = c;
                continue;
            }                            

            auto const& [of, gW, gH, xAdv, xOff, yOff] = m_template.font->glyph[int(c) - m_template.font->first];
            c = (lastC == 208 && c == 129) ? 192 :      // recoding 'Ë' letter
                ((lastC == 209 && c == 145) ? 193 : c); // recoding 'ё' letter

            drawGlyph(
                x + m_x + xOff,
                m_y - yOff, 
                m_template.letterCol,
                m_template.font->glyph[int(c) - m_template.font->first],
                m_template.font);

            lastC = 0;
            
            x += xAdv;
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

    size_t _getTextWidthPx(const std::string& str)
    {
        char lastC = 0;
        size_t widthPx = 0;
        for(char c : str)
        {
            if(c > 200)
            {
                lastC = c;
                continue;
            }                            

            auto const& [of, gW, gH, xAdv, xOff, yOff] = m_template.font->glyph[int(c) - m_template.font->first];
            c = (lastC == 208 && c == 129) ? 192 :      // recoding 'Ë' letter
                ((lastC == 209 && c == 145) ? 193 : c); // recoding 'ё' letter
            
            widthPx += m_template.font->glyph[int(c) - m_template.font->first].xAdvance;
        }

        return widthPx;
    }

public:
    TextEffect(Framebuffer* fb, TextTemplate textTemplate)
        :
        AbstractEffect{fb, EffectsEnum::TEXT},
        m_template{textTemplate},
        m_speed{100},

        // m_fb is a pointer to a framebuffer, it could be null, so it is important to handle such corner cases
        m_x{m_fb ? (int)m_fb->getWidth() : 0},
        m_y{7},
        
        m_filter{nullptr},
        m_filterFb{
            m_fb ? m_fb->getWidth() : 2, 
            m_fb ? m_fb->getHeight() : 2, 
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
            TEXT_EF_LOG("text_effect", "Text set to %s", m_template.text.c_str());
        }

        if(const auto it = props.find("letterCol"); it != props.end())
        {
            m_template.letterCol = std::stoul(it->second, 0, 16);
            TEXT_EF_LOG("text_effect", "letterCol set to %lu", m_template.letterCol);
        }

        if(const auto it = props.find("backCol"); it != props.end())
        {
            m_template.backCol = std::stoul(it->second, 0, 16);
            TEXT_EF_LOG("text_effect", "backCol set to %lu", m_template.backCol);
        }

        if(const auto it = props.find("displayTime"); it != props.end())
        {
            m_template.displayTime = std::stoul(it->second);
            TEXT_EF_LOG("text_effect", "displayTime set to %u", m_template.displayTime);
        }

        if(const auto it = props.find("scrollTimes"); it != props.end())
        {
            m_template.scrollTimes = std::stod(it->second);
            m_scrollsLeft = m_template.scrollTimes;
            TEXT_EF_LOG("text_effect", "scrollTimes set to %d", m_template.scrollTimes);
        }

        // if(const auto it = props.find("textFilter"); it != props.end())
        // {
        //     m_template.textFilter = EffectsEnum(std::stoul(it->second));
        //     TEXT_EF_LOG("text_effect", "textFilter set to %d", int(m_template.textFilter));
        // }
    } 

    void setTextTemplate(TextTemplate textTemplate)
    {
        m_template = textTemplate;
        m_scrollsLeft = textTemplate.scrollTimes;
        m_x = m_fb ? (int)m_fb->getWidth() : 0;
        this->setFilter(textTemplate.textFilter, textTemplate.textFilterProps);
    }

    void setFrameBuffer(Framebuffer* fb) override
    {
        m_fb = fb;

        // Update the member's value, that depends on Framebuffer's props
        m_x = (int)m_fb->getWidth();
        // m_y = (int)(m_fb->getHeight() - m_font5x7.getCharHeight()) / 2;
        m_y = 7;
        
        m_filterFb.resize(m_fb->getWidth(), m_fb->getHeight());
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
        TEXT_EF_LOG("textEffect", "Constructing m_filter");
        m_filter = EffectFactory::getEffect(&m_filterFb, filterEnum, props);
        TEXT_EF_LOG("textEffect", "m_filter constructed");
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
            
            // Text width in pixels
            const int textWidthPx = this->_getTextWidthPx(m_template.text);

            //TEXT_EF_LOG("text_effect", "tmr is %lld; x is %d, textWidthPx is %d", tmr, m_x, textWidthPx);

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
                // TEXT_EF_LOG("text", "Text is %s, scrollsLeft: %u", m_template.text.c_str(), m_scrollsLeft);
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