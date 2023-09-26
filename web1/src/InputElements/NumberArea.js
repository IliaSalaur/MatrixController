import { useState } from "react";

function NumberArea(propName, {def, name, columns, min, max}, changeCB) {
    return (
        <div className="wrap textarea-wrap numberarea-wrap" key={`number-${propName}`} style={{gridColumn: columns}}>
            <label htmlFor={`number-${propName}-id`} className="input-label">{name}</label>
            <input 
                type="number"
                min={min}
                max={max}
                aria-colcount={100}
                aria-rowcount={1}
                className="textarea-input"
                name={`number-${propName}`} 
                id={`number-${propName}-id`} 
                value={def}
                // required={false}
                onInput={(e)=>{
                    console.log(e.target.value);
                    let num = Number(e.target.value);

                    num = max !== undefined ? (num <= max ? num : max) : num;
                    num = min !== undefined ? (num >= min ? num : min) : num;

                    changeCB(e.target.value === '' || e.target.value === '-' ? e.target.value : num, propName);
                }}
            />
        </div>
    );
}

export default NumberArea;