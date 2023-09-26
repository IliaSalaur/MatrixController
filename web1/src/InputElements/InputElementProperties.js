import ColorPicker from "./ColorPicker"
import Slider from "./Slider"
import TextArea from "./TextArea"
import Select from "./Select"
import TimePicker from "./TimePicker"
import CheckBox from "./CheckBox"
import NumberArea from "./NumberArea"
import Button from "./Button"

export const InputElementType = {
    Slider: Slider, ColorPicker: ColorPicker, TextArea: TextArea, Select: Select, TimePicker: TimePicker, CheckBox: CheckBox, NumberArea:NumberArea, Button:Button
}

export const InputElements = {
    // Common
    scale: { create: InputElementType.Slider, name: 'Scale'},
    speed: { create: InputElementType.Slider, name: 'Speed'},

    //Fire
    palette: {create: InputElementType.Select, name: 'Palette'},

    //Matrix
    density: {create: InputElementType.Slider, name: 'Density'},

    //Rainbow:
    direction: {create: InputElementType.Select, name: 'Direction'},

    //Waterfall
    hue: {create: InputElementType.Slider, name: 'Hue'},
    cooling: {create: InputElementType.Slider, name: 'Cooling'},
    sparking: {create: InputElementType.Slider, name:'Sparking'},
    mode: {create: InputElementType.Select, name: 'Mode'},

    //Text
    letterCol: { create: InputElementType.ColorPicker, name: 'Letter Color'},
    backCol: { create: InputElementType.ColorPicker, name: 'Background Color'},
    scroll: { create: InputElementType.Slider, name: 'Scroll'},
    text: { create: InputElementType.TextArea, name: 'Text'},
    textFilter:{create: InputElementType.Select, name: 'Filter'},
    displayTime:{create: InputElementType.NumberArea, name: 'Display Time'},
    scrollTimes:{create: InputElementType.NumberArea, name: 'Scroll times'},

    //Text Control Panel
    delete:{create:InputElementType.Button, name: 'Delete'},
    save:{create:InputElementType.Button, name: 'Save'},
    add:{create:InputElementType.Button, name: 'Add'},

    //Main menu
    brig:{create:InputElementType.Slider, name: 'Brightness'},

    //Sync
    childMatrix:{create:InputElementType.Select, name: 'Add as child'},
    connect:{create:InputElementType.Button, name: 'Connect'}
}