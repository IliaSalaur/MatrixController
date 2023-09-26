import { useContext, useRef, useState } from "react";
import useLocalStorage from '../useLocalStorage'
import CardCollection from "../CardCollection";
import TextSequenceUI from "./TextSequenceUI";
import { CSSTransition } from "react-transition-group";
import { EffectCards } from "../EffectsCollection";
import add from "../img/add.svg"
import * as requests from "../requests" 
import TextSequencePreview from "./TextSequencePreview";
import ControlUI from "../InputElements/ControlUI";
import { DevicesContext } from "../DevicesContext";

import close from '../img/close.svg'

const effectCardsToFilterOptions = () =>{
    let newObj = [{title: 'No filter', value: 99}];
    EffectCards.forEach((el, index)=>{
        newObj.push({title:el.cardTitle, value:Number(index)});
    });

    return newObj;    
}

const FilterOptions = effectCardsToFilterOptions();

// * @param {{text:string, letterCol:int, backCol:int, textFilter:int, displayTime:int, scrollTimes:int}} template 
const TextSequencesDefault = [
    [   
        {text: {def:'Small template'}, letterCol: {def:'#ff0000'}, backCol: {def:'#908070'}, displayTime: {def: 3}/* seconds*/, scrollTimes:{def:1}, textFilter: {options:FilterOptions, def:99}, textFilterProps:{}},
    ]
];

const TextSequenceCollection = () => {    
    const nodeRef = useRef(null);
    const { selectedDevice, devices }= useContext(DevicesContext);
    const [showingIndex, setShowingIndex] = useState(null);
    const [defaultProps, setDefaultProps] = useLocalStorage(`${selectedDevice}:seqs`, TextSequencesDefault);
    // const [defaultProps, setDefaultProps] = useState(TextSequencesDefault);

    // Create an object to store all changes
    // let changedProperties = JSON.parse(JSON.stringify(defaultProps));//old
    let changedProperties = structuredClone(defaultProps);

    const getDevice = ()=>{
        const newDevs = devices.filter(dev => dev.id === selectedDevice);
        return newDevs[0];
    }

    /**
     * 
     * @param {*} textSequences Array of text sequences
     * @returns cards
     */
    const textSequencesToCards = (textSequences) => {
        let cards = [];     
        textSequences.forEach((el, index)=>{
            let shortTitle = el[0].text.def.substring(0, 18);            
            shortTitle += el[0].text.def.length > 18 ? '...' : '';
            cards.push({cardTitle:shortTitle, cardImg:<TextSequencePreview sequence={el}/>, cardName:index, elementAsImg:true});
        });
        cards.push({cardTitle:'Add', cardImg:add, cardName:'add-template'});
        return cards;
    }

    const textSequenceUiFromTextIndex = (templateIndex) => {
        return (
        <TextSequenceUI 
            key={`${new Date().getTime()}:${templateIndex}`}
            controlElements={getControlElementsFromIndex(templateIndex)}
            onChangeCb={(newObj)=>{handlePropertiesChange(newObj, showingIndex, templateIndex)}}
        />);
    }

    const handleOnCLick = (e, sequenceIndex) => {        
        if(sequenceIndex === 'add-template'){
            let newIndex = defaultProps.length;
            setDefaultProps(
                [
                    ...defaultProps,
                    [{text: {def:''}, letterCol: {def:'#909090'}, backCol: {def:'#454545'}, displayTime: {def: 1}, scrollTimes:{def:1}, textFilter: {options:FilterOptions, def:99}, textFilterProps:{}}]
                ]
            );
            setShowingIndex(newIndex);            
        }
        else{            
            requests.setTextTemplate(defaultProps[sequenceIndex], getDevice());
        }
    }    

    const handleShowUI = (e, index) => {
        // e.preventDefault();
        setShowingIndex(index);
    }

    const handleCloseUI = (e) => {        
        if(e.target.className === 'template-ui-wrapper' || e.target.classList.contains('template-close-icon') || e.target.value === "Save")    
        {
            // e.preventDefault();
            setShowingIndex(null);
            setDefaultProps(changedProperties);
        }                        
    }

    const handlePropertiesChange = (newProps, sequenceIndex, templateIndex) => { 
        if(newProps.delete.def === true)
        {                                            
            if(changedProperties[sequenceIndex].length === 1)
            {
                changedProperties.splice(sequenceIndex, 1);
                setShowingIndex(null);
            }
            else{
                changedProperties[sequenceIndex].splice(templateIndex, 1);                  
            }   
            setDefaultProps(changedProperties);         
        }
        else{
            // Catch the change, but don't rerender
            changedProperties[sequenceIndex][templateIndex] = newProps;
        }
    }

    const addTemplate = () => {
        changedProperties[showingIndex].push({text: {def:''}, letterCol: {def:'#909090'}, backCol: {def:'#454545'}, displayTime: {def: 1}, scrollTimes:{def:1}, textFilter: {options:FilterOptions, def:99}, textFilterProps:{}})
        setDefaultProps(changedProperties);
    }

    const getControlElementsFromIndex = (textIndex) => {
        return {...changedProperties[showingIndex][textIndex], delete:{def:false, color:'#ff0000'}};    
    }


    const body = document.getElementsByTagName('body')[0];
    body.style.overflowY = showingIndex === null ? "scroll" : "hidden";
    return (  
        <>
            <CardCollection cards={textSequencesToCards(defaultProps)} onClick={(e, index)=>{handleOnCLick(e, index)}} onContextMenu={(e, index)=>{handleShowUI(e, index)}}/>
            <CSSTransition
                in={showingIndex !== null}
                nodeRef={nodeRef}
                timeout={200}
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
                    <span className="drawer-close-icon template-close-icon" onClick={handleCloseUI}><img className="bw-icon template-close-icon" src={close} alt="" /></span>
                        <div className="template-relative">
                        {showingIndex !== null && 
                            changedProperties[showingIndex].map((el, i) => textSequenceUiFromTextIndex(i))
                        }
                        <ControlUI 
                            key={"save"}
                            controlElements={{save:{color:'#00aa00', submit:true}}}
                            onChangeCb={(newObj)=>{}}
                            onSubmitCb={(e) => {handleCloseUI(e)}}                            
                        />
                        <ControlUI 
                            key={"add"}
                            controlElements={{add:{color:'#2a2a2a7a'}}}
                            onChangeCb={(newObj)=>{addTemplate()}}
                        />
                        </div>
                    </div>
                </div>
            </CSSTransition>
        </>

    );
}
 
export default TextSequenceCollection;