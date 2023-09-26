import { Fragment, useEffect, useState} from "react";
import { InputElements } from "./InputElementProperties";

/**
 * 
 * @param {propName:{def, name}, propName:{def, name}} controlElements 
 * @returns 
 */
function ControlUI(props) {        
    const { controlElements, onChangeCb, onSubmitCb, onUnmountCb, CustomInputElements} = props;
    const [state, setState] = useState(controlElements);
    const controlElementsKeys = Object.keys(controlElements);

    let inpElems = CustomInputElements === undefined ? InputElements : CustomInputElements;

    const onchangeCB = (val, propName) =>{
        console.log(`val:${val}  propName:${propName}`);

        let newObj = {};
        
        if(onChangeCb === undefined) newObj = {...state};
        else newObj = {...controlElements};

        newObj[propName].def = val;
        
        if(onChangeCb === undefined) setState(newObj);
        else  onChangeCb(newObj);        
    }

    useEffect(() => () => {
        if(onUnmountCb) onUnmountCb();
    });

    return ( 
    <form className="control-ui" onSubmit={onSubmitCb}>
        {controlElementsKeys.map((propName) =>{
            if(!inpElems.hasOwnProperty(propName)) return <Fragment key={propName}></Fragment>;
            return inpElems[propName].create(propName, {...controlElements[propName], name:inpElems[propName].name}, onchangeCB)
        })}
    </form>
    );
}

export default ControlUI;