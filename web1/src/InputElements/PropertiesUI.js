function SpoilerPropertiesUI(propName, {options, def, name}, changeCB) {
    return ( 
        <>
            <form className="control-ui" onSubmit={onSubmitCb}>
                {controlElementsKeys.map((propName) =>{
                    if(!InputElements.hasOwnProperty(propName)) return <Fragment key={propName}></Fragment>;
                    return InputElements[propName].create(propName, {...controlElements[propName], name:InputElements[propName].name}, onchangeCB)
                })}
            </form>
        </>
    );
}

export default Select;