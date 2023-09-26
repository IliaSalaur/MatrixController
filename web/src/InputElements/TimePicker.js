function TimePicker(propName, {def, name}, changeCB) {    
    return (
    <div key={`timepicker-${propName}`} className="wrap timepicker-wrap">
        <label htmlFor={`timepicker-${propName}-id`} className="input-label">{name}</label>
        <input         
            type="time" 
            className="timepicker-input"
            name={`timepicker-${propName}`} 
            id={`timepicker-${propName}-id`} 
            value={def}

            onChange={(e)=>{
                changeCB(e.target.value, propName);
            }}
        />
    </div>
    );
}

export default TimePicker;