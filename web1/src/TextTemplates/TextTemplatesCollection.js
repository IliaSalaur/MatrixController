import { useRef } from "react";
import useLocalStorage from '../useLocalStorage'
import CardCollection from "../CardCollection";
import ControlUI from "../InputElements/ControlUI";
import TextTemplatePreview from "./TextTemplatePreview";
import { CSSTransition } from "react-transition-group";
import { EffectCards } from "../EffectsCollection";
import Add from "../img/Add.svg"
import * as requests from "../requests" 

const effectCardsToFilterOptions = () =>{
    let newObj = [{title: 'No filter', value: '99'}];
    EffectCards.forEach((el, index)=>{
        newObj.push({title:el.cardTitle, value:index});
    });
    return newObj;
}

const FilterOptions = effectCardsToFilterOptions();

// * @param {{text:string, letterCol:int, backCol:int, textFilter:int, displayTime:int, scrollTimes:int}} template 
const TextTemplatesDefault = [
    {text: {def:'Hello this is text'}, letterCol: {def:'#ff0000'}, backCol: {def:'#ff000000'}, displayTime: {def: 3}/* seconds*/, scrollTimes:{def:1}, textFilter: {options:FilterOptions, def:0}},
];

const TextTemplateCollection = () => {    
    const [state, setState] = useLocalStorage('TextTemplates', {showing:null, templatesProps:TextTemplatesDefault});
    const nodeRef = useRef(null);
    const {showing, templatesProps} = state;

    console.log(state);

    const textTemplatesToCards = (textTemplates) => {
        let cards = [];        
        textTemplates.forEach((el, index)=>{
            let shortTitle = el.text.def.substring(0, 18);            
            shortTitle += el.text.def.length > 18 ? '...' : '';
            cards.push({cardTitle:shortTitle, cardImg:<TextTemplatePreview textTemplate={el}/>, cardName:index, elementAsImg:true});
        });
        cards.push({cardTitle:'Add', cardImg:Add, cardName:'add-template'});
        return cards;
    }

    const handleShowUI = (event, templateIndex) =>{
        if(templateIndex === 'add-template') return;
        event.preventDefault();
        setState({...state, showing:templateIndex});
    }

    const handleCloseUI = (e) => {
        if(e.target.className === 'template-ui-wrapper')  setState({...state, showing:null});    
        // localStorage.setItem('TextTemplates', JSON.stringify(templatesProps));
    }

    const handleOnCLick = (e, index) =>{
        if(index === 'add-template'){
            let newIndex = templatesProps.length;
            let newProps = [
                ...templatesProps,
            ];
            newProps[newIndex] = {text: {def:''}, letterCol: {def:'#ffffff'}, backCol: {def:'#ff000000'}, displayTime: {def: 1}, scrollTimes:{def:1}, textFilter: {options:FilterOptions, def:99}};
            setState({
                showing:newIndex, 
                templatesProps:newProps,
            });
        }
        else{
            requests.setTextTemplate(templatesProps[index]);
        }
    }

    const handlePropertiesChange = (newObj, index) => {                
        let newProps = [
            ...templatesProps,
        ];

        if(newObj.delete.def === true){
            newProps.pop(index);
            setState({showing:null, templatesProps:newProps});
        }
        else {
            newProps[index] = newObj;
            delete newProps[index].delete;
            delete newProps[index].save;
            setState({...state, templatesProps:newProps});
        }                        
    }

    const getTemplateProps = (index) =>{
        return {...templatesProps[index], save:{def:false, color:'#00aa00', submit:true}, delete:{def:false, color:'#ff0000'}};
    }

    const handleSubmit = (e)=>{
        e.preventDefault();
        setState({...state, showing:null})
    }

    // useEffect(()=>{
    //     let data = localStorage.getItem('TextTemplates');
    //     if(data !== null) setState({...state, templatesProps:JSON.parse(data)})
    // }, []);

    return (  
        <>
            <CardCollection cards={textTemplatesToCards(templatesProps)} onClick={(e, index)=>{handleOnCLick(e, index)}} onContextMenu={(e, index)=>{handleShowUI(e, index)}}/>
            <CSSTransition
                in={showing !== null}
                nodeRef={nodeRef}
                timeout={200}
                // onEnter={() => setDrawerState(true)}
                // onExited={() => setDrawerState(false)}
                classNames={{
                    enter:'template-ui-enter',
                    enterActive:'template-ui-enter-active',
                    exit:'template-ui-exit',
                    exitActive:'template-ui-exit-active'
                }}
                unmountOnExit
            >
                <div ref={nodeRef} className="template-ui-wrapper" onClick={handleCloseUI}>
                    <div className="template-ui">
                        {showing !== null && 
                            <ControlUI 
                            controlElements={getTemplateProps(showing)} 
                            /*onUnmounCb={(newObj)=>{handlePropertiesChange(newObj, showing)}}*/ 
                            onChangeCb={(newObj)=>{handlePropertiesChange(newObj, showing)}}
                            onSubmitCb={handleSubmit}/>}
                    </div>
                </div>
            </CSSTransition>
        </>

    );
}
 
export default TextTemplateCollection;