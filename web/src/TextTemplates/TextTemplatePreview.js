import { EffectCards } from "../EffectsCollection";

const TextTemplatePreview = (props) => {
    const template = props.textTemplate;
    const styles = props.style;

    const getBackgroundImg = (col) =>{
        if(col === '#ff000000')
        {
            return template.textFilter.def == '99' ? 'transparent' : `url(${EffectCards[template.textFilter.def].cardImg})`;
        }
        return col;
    }

    let backCol = getBackgroundImg(template.backCol.def);

    return ( 
        <div 
            className="card-img preview-img" 
            style={{...styles, background: backCol}}
        >
            <p 
                className={`preview-text ${template.letterCol.def === '#ff000000' ? 'preview-clip' : ''}`} 
                style={
                        {
                            color:template.letterCol.def === '#ff000000' ? 'transparent' : template.letterCol.def, 
                            backgroundImage: template.letterCol.def === '#ff000000' ? (template.textFilter.def == '99' ? 'none' : `url(${EffectCards[template.textFilter.def].cardImg})`) : 'none'
                        }
                    }
                >
                    {template.text.def}
                </p>
        </div>
    );
}
 
export default TextTemplatePreview;