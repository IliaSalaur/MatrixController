function CheckBox(propName, {def, name}, changeCB) {
    return (
        <div key={`checkbox-${propName}`} className="wrap checkbox-wrap">
            <label htmlFor={`checkbox-${propName}-id`} className="input-label">{name}</label>
            <input 
                type="checkbox" 
                className="checkbox-input"
                name={`checkbox-${propName}`} 
                id={`checkbox-${propName}-id`} 
                checked={def === 'true' ? true : (def === 'false' ? false : (!isNaN(Number(def)) ? Number(def) : def))}
                onChange={(e)=>{
                    changeCB(e.target.checked, propName);
                }}
            />
        </div>
    );
}

export default CheckBox;