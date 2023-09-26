import TextTemplatePreview from "./TextTemplatePreview";

const TextSequencePreview = (props) => {
    const {sequence} = props;
    
    // const cssWidth = getComputedStyle(document.documentElement).getPropertyValue('--card-width');
    // const previewWidth = getComputedStyle(document.documentElement).getPropertyValue('--preview-img-width');
    const cssWidth = 155;
    const previewWidth = 50;

    const sequenceElOffset = (cssWidth / sequence.length) - (previewWidth / (sequence.length > 1 ? sequence.length - 1 : 1));
    const sequenceElSize = (cssWidth / sequence.length) + (sequence.length > 1 ? previewWidth : 0);
    const maxZIndex = 100;

    return ( 
    <div className="text-sequence">
        {sequence.map((el, i) => 
            <TextTemplatePreview style={{
                left: i * sequenceElOffset,
                bottom: i * sequenceElOffset,
                width: sequenceElSize,
                height: sequenceElSize,
                zIndex: maxZIndex - i,
                fontSize: sequenceElSize
            }} textTemplate={el} key={i}/>)}
    </div>
    );
}
 
export default TextSequencePreview;