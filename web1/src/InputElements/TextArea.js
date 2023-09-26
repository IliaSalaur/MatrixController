function TextArea(propName, {def, name}, changeCB) {
    return (
        <div className="wrap textarea-wrap" key={`textarea-${propName}`}>
            <label htmlFor={`textarea-${propName}-id`} className="input-label">{name}</label>
            <input 
                type="text" 
                aria-colcount={100}
                aria-rowcount={1}
                className="textarea-input"
                name={`textarea-${propName}`} 
                id={`textarea-${propName}-id`} 
                value={def}
                onChange={(e)=>{
                    changeCB(e.target.value, propName);
                }}
            />
        </div>
    );
}

export default TextArea;