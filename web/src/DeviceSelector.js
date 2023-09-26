import { useEffect, useState } from "react";
import * as requests from "./requests";
import ControlUI from "./InputElements/ControlUI";

function DeviceSelector({handleSelect, devices, selectedDevice }) {
    const [brigObj, setBrig] = useState({[selectedDevice]:20});

    const getDevice = ()=>{
        const newDevs = devices.filter(dev => dev.id == selectedDevice);
        return newDevs.length === 0 ? {} : newDevs[0];
    }

    const handleBrigChange = (newObj) =>{
        console.log("cb newObj", newObj);
        requests.setConfigs(newObj, getDevice());

        setBrig(
            {
                ...brigObj,
                [selectedDevice]:newObj.brig.def
            }
        );
    }

    useEffect(()=>{
        if(selectedDevice in brigObj)
            return;

        setBrig(
            {
                ...brigObj,
                [selectedDevice]:20
            }
        );
    }, [selectedDevice]);

    return ( 
    <>
    <div className="wrap select-wrap">
        {/* <label htmlFor={`select-devices-id`} className="input-label">Devices</label> */}
        <select 
            name={`select-devices`} 
            id={`select-devices-id`} 
            className="select-input"
            value={selectedDevice || '0'}
            onChange={handleSelect}
        >
            {devices.map((option)=>{
                return <option key={option.id} value={option.id} className="select-option">{option.tag}</option>
            })}
        </select>
    </div>
    <ControlUI 
        controlElements={{brig:{min:0, def:brigObj[selectedDevice] || 0, max:255}}}
        onChangeCb={handleBrigChange}
    />
    </>
    );
}

export default DeviceSelector;