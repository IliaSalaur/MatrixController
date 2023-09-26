const Button = (propName, {def, name, color, submit}, changeCB) => {
    return ( 
        <input 
            type={submit !== undefined ? 'submit' : 'button'}
            style={{backgroundColor:color}}
            onClick={(e)=>{
                changeCB(true, propName);
            }}
            className="button"
            key={`button-${propName}`}
            value={name}
        >            
        </input>
    );
}
 
export default Button;