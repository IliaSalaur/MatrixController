function Slider(propName, {min, max, def, name}, changeCB) {    
    return (
    <div key={`slider-${propName}`} className="wrap slider-wrap">
        <label htmlFor={`slider-${propName}-id`} className="input-label">{name}</label>
        <input         
            type="range" 
            className="slider-input"
            name={`slider-${propName}`} 
            id={`slider-${propName}-id`} 
            min={min} 
            max={max}
            value={def}
            onChange={(e)=>{
                changeCB(e.target.value, propName);
            }}
        />
    </div>
    );
}

export default Slider;