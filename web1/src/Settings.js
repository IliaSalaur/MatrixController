import { InputElementType } from "./InputElements/InputElementProperties";
import ControlUI from "./InputElements/ControlUI";
import { useContext, useEffect, useState } from "react";
import * as requests from "./requests";
import { DevicesContext } from "./DevicesContext";

const SettingsInputElements = {
    ssid:{create:InputElementType.TextArea, name: 'SSID'},
    pass:{create:InputElementType.TextArea, name: 'Password'},
    enableAP:{create:InputElementType.CheckBox, name: 'Enable Access Point'},
    timezone:{create:InputElementType.NumberArea, name: 'Timezone (in sec.)'},
    width:{create:InputElementType.NumberArea, name: 'Width'},
    height:{create:InputElementType.NumberArea, name: 'Height'},
    tag:{create:InputElementType.TextArea, name: 'Tag'}
};

const WlanSettingsList = {
    ssid:{def: ''},
    pass:{def: ''},
    enableAP:{def: false}
};

const TimeSettingsList = {
    timezone:{def:0, columns:'span 12', min:-43200, max:43200}
};

const MatrixSettingsList = {
    width:{def: 16, max:255, min:0, columns: 'span 6'},
    height:{def: 16, max:255, min:0, columns: 'span 6'},
    tag:{def:'wsc-no'}
};

function Settings() {
    const [configs, setConfigs] = useState({...WlanSettingsList, ...TimeSettingsList, ...MatrixSettingsList});
    const { selectedDevice, devices } = useContext(DevicesContext);

    const getDevice = ()=>{
        const newDevs = devices.filter(dev => dev.id == selectedDevice);
        return newDevs.length === 0 ? {} : newDevs[0];
    }

    const getControlElements = (defaultElems) =>{
        let elems = {};
        const keys = Object.keys(defaultElems);
        keys.forEach((key, i)=>{
            elems[key] = {...defaultElems[key]};
            elems[key].def = key in configs ? configs[key].def : elems[key].def;
        }); 
        console.log(elems)
        return elems;
    }

    useEffect(() =>{
        const getConfigs = async() =>{          
            try{                                
                const device = getDevice();
                console.log('devices are', devices);
                console.log('selected is', selectedDevice);
                
                const url = 'http://' + device.ip + '/getConfigs'; 
                const res = await fetch(url);
                const data = await res.json();
                
                console.log(data);
                const keys = Object.keys(data);
                let newConfs = {};
                keys.forEach(key =>{
                    newConfs[key] = {def:data[key]};
                }); 
                setConfigs(newConfs);
                // const keys = Object.keys(data);
                // let newConfs = JSON.parse(JSON.stringify({...WlanSettingsList, ...TimeSettingsList}));
                // keys.forEach(key =>{
                //     if(key in newConfs)newConfs[key].def = data[key];      
                // });
                // console.log(newConfs);
                // setConfigs(newConfs);                
            }
            catch(e){
                console.log(e);
                console.log(TimeSettingsList)
                setConfigs({});
                // setTimeout(()=> getConfigs(getDevice()), 500);
            }
        }

        console.log('fetching', selectedDevice);

        if('ip' in getDevice())
        {
            getConfigs();
        }
    }, [selectedDevice])

    const handleChange = (newProps) =>{
        const newObj = {...configs, ...newProps};
        console.log(newObj);
        setConfigs(newObj);
    }

    const handleCloseUI = (e) => {        
        // if(e.target.className === 'template-ui-wrapper' || e.target.classList.contains('template-close-icon') || e.target.value === "Save")    
        // {
            e.preventDefault();
            requests.setConfigs(configs, getDevice());
        // }                        
    }

    return ( 
        <>
        <div className="card settings-card">
            <h3 className="card-title settings-card-title">WLAN</h3>
            <ControlUI name="coUI" onChangeCb={handleChange} controlElements={getControlElements(WlanSettingsList)} CustomInputElements={SettingsInputElements}/>
        </div>            

        <div className="card settings-card">
            <h3 className="card-title settings-card-title">Time</h3>
            <ControlUI name="coUI" onChangeCb={handleChange} controlElements={getControlElements(TimeSettingsList)} CustomInputElements={SettingsInputElements}/>
        </div>

        <div className="card settings-card">
            <h3 className="card-title settings-card-title">Matrix</h3>
            <ControlUI name="coUI" onChangeCb={handleChange} controlElements={getControlElements(MatrixSettingsList)} CustomInputElements={SettingsInputElements}/>
        </div>     
        <ControlUI 
            key={"save"}
            controlElements={{save:{color:'#00aa00', submit:true}}}
            onChangeCb={(newObj)=>{}}
            onSubmitCb={(e) => {handleCloseUI(e)}}                            
        />
        </>
    );
}

export default Settings;