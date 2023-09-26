import { Fragment, useEffect, useState} from "react";
import { InputElements } from "../InputElements/InputElementProperties";
import { Effects } from "../EffectsCollection"

import chevron from '../img/chevron.svg'

/**
 * 
 * @param {propName:{def, name}, propName:{def, name}} controlElements 
 * @returns 
 */
function TextSequenceUI(props) {        
    const { controlElements, onChangeCb, onSubmitCb, onUnmountCb} = props;
    const [state, setState] = useState(controlElements);
    const [showFilterProps, setFilterProps] = useState(false);
    const controlElementsKeys = Object.keys(controlElements);

    const onchangeCB = (val, propName) =>{
        // console.log(`val:${val}  propName:${propName}`);

        // let newObj = {};
        let newObj = structuredClone(state);
        newObj.textFilterProps = structuredClone(state.textFilterProps);
        
        // /*if(onChangeCb === undefined)*/ newObj = {...state, textFilterProps:state.textFilterProps};

        newObj[propName].def = val;

        if(propName === 'textFilter')
        {
            const efKeys = Object.keys(Effects);
            newObj.textFilterProps = val == 99 ? {} : {...Effects[efKeys[Number(val)]]};
            console.log("New props generated")
        }
        
        onChangeCb(newObj);
        setState(newObj);        
    }

    const onFilterChangeCB = (val, propName) =>{
        let newObj = {};
        
        /*if(onChangeCb === undefined)*/ newObj = {...state, textFilterProps:state.textFilterProps};

        newObj.textFilterProps[propName].def = val;
        console.log(newObj);
        
        onChangeCb(newObj);
        setState(newObj);
    }

    useEffect(() => () => {
        if(onUnmountCb) onUnmountCb();
    });

    const handleSpoilerClick = (e) =>{
        setFilterProps(!showFilterProps);
    }

    const displayFilterProps = () =>{
        const props = state.textFilterProps;
        const keys = Object.keys(props);
        
        console.log(props);

        return keys.map((propName)=> {
            if(!InputElements.hasOwnProperty(propName)) return <Fragment key={`filter-${propName}`}></Fragment>;
            return InputElements[propName].create(propName, {...props[propName], name:InputElements[propName].name}, onFilterChangeCB)
        });
    }

    return ( 
    <form className="control-ui sequence-ui" onSubmit={onSubmitCb}>
        {controlElementsKeys.map((propName) =>{
            if(!InputElements.hasOwnProperty(propName)) return <Fragment key={propName}></Fragment>;
            return InputElements[propName].create(propName, {...state[propName], name:InputElements[propName].name}, onchangeCB)
        })}
        <div className="filter-wrap">
            <span className="input-label" onClick={handleSpoilerClick}>Filter Props</span>
            <span className="material-icons filter-icon" onClick={handleSpoilerClick}><img style={{transform: `rotate(${showFilterProps ? 90 : 0}deg)`}} className="bw-icon" src={chevron} alt="" /></span>
        </div>
        {
            showFilterProps && 
             displayFilterProps()
        }
        {console.log("rerender")}
    </form>
    );
}

export default TextSequenceUI;