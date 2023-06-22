#ifndef MY_TEXT_SEQUENCE_HPP
#define MY_TEXT_SEQUENCE_HPP

#include <string>
#include <memory>
#include <vector>

#include "Effects/Framebuffer.hpp"
#include "Effects/AbstractEffect.hpp"
#include "Effects/EffectFactory.hpp" 
#include "TextEffect.hpp"

class TextSequenceEffect : public AbstractEffect
{
private:
    std::vector<TextTemplate> m_sequence;
    TextEffect m_textEffect;
    size_t m_templateIndex;

    void _showSequence(size_t templateIndex)
    {
        if(templateIndex >= m_sequence.size())
        {
            m_textEffect.setTextTemplate({});
            return;
        }

        m_textEffect.setTextTemplate(m_sequence[templateIndex]);
    }

public:
    TextSequenceEffect(Framebuffer* fb, const std::vector<TextTemplate>& sequence)
        :
        AbstractEffect{fb, EffectsEnum::TEXT_SEQUENCE},
        m_sequence{sequence},
        m_textEffect{fb, {}},
        m_templateIndex{0}
    {
        this->_showSequence(m_templateIndex);
    }

    void setPropertiesFromMap(const effect_properties_t& props) override
    {
        
    }

    /**
     * @brief Set the TextSequence to be shown
     * 
     * @param sequence a vector of TextTemplates
     */
    void setTextSequence(std::vector<TextTemplate> sequence)
    {
        m_sequence = sequence;
        m_templateIndex = 0;
        this->_showSequence(0);
    }

    esp_err_t generateFrame() override
    {
        esp_err_t res = m_textEffect.generateFrame();

        if(!m_sequence.empty() && m_textEffect.getSrollsLeft() == 0)
        {
            m_templateIndex = m_templateIndex + 1 < m_sequence.size() ? m_templateIndex + 1 : 0;
            ESP_LOGI("seq", "Current index is %u", m_templateIndex);
            this->_showSequence(m_templateIndex);
        }

        return res;
    }
};

#endif