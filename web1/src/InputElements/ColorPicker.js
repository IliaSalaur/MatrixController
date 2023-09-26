import palette from '../img/palette.svg'

function ColorPicker(propName, {def, name}, changeCB) {
    return (
        <div key={`colorpicker-${propName}`} className="wrap colorpicker-wrap" style={{}}>
            <label htmlFor={`colorpicker-${propName}-id`} className="input-label">{name}</label>

            <div className="colorpicker-panel" style={{'backgroundColor': def == '#ff000000' ? '#909090' : def}}>
                <span className="material-icons colorpicker-palette"><img style={{filter:def == '#ff000000' ? 'none' : 'invert()'}} src={palette} alt="" /></span>
                <input
                    type="color"
                    className="colorpicker-input"
                    name={`colorpicker-${propName}`}
                    id={`colorpicker-${propName}-id`}
                    value={def == '#ff000000' ? '#000000' : def}
                    onChange={(e)=>{
                        changeCB(e.target.value, propName);
                    }}
                    onContextMenu={(e)=>{
                        e.preventDefault();
                        changeCB('#ff000000', propName);
                    }}
                />
            </div>
        </div>
    );
}

export default ColorPicker;