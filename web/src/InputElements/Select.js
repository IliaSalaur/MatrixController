function Select(propName, {options, def, name}, changeCB) {
    return ( 
    <div key={`select-${propName}`} className="wrap select-wrap">
        <label htmlFor={`select-${propName}-id`} className="input-label">{name}</label>
        <select 
            name={`select-${propName}`} 
            id={`select-${propName}-id`} 
            className="select-input"
            value={def}
            onChange={(e)=>{
                changeCB(e.target.value, propName);
            }}
        >
            {options.map((option)=>{
                return <option key={option.value} value={option.value} disabled={'disabled' in option ? option.disabled : false} className="select-option">{option.title}</option>
            })}
        </select>
    </div>
    );
}

export default Select;