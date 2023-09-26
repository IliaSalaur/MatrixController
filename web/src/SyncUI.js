import { useContext, useEffect, useState } from "react";
import { DevicesContext } from "./DevicesContext";
import ControlUI from "./InputElements/ControlUI";
import * as requests from './requests'

function SynchonisationUI() {
    const { selectedDevice, devices } = useContext(DevicesContext);
    const [childDevice, setChildDevice] = useState(-1);

    const getDevice = (devId)=>{
        const newDevs = devices.filter(dev => dev.id == devId);
        return newDevs.length === 0 ? {} : newDevs[0];
    }
    
    useEffect(()=>{
        setChildDevice(devices.length > 1 ? devices.filter(dev => dev.id != selectedDevice)[0].id : -1);
    }, [devices, selectedDevice]);

    const handleChange = (newProps) =>{
        const {connect, childMatrix} = newProps;
        console.log(newProps);

        if(connect.def === true)
        {
            console.log("registering as Child");
            requests.registerAsChild(getDevice(childDevice), getDevice(selectedDevice));
            connect.def = false;
            return;
        }

        if(childMatrix.def !== childDevice)
            setChildDevice(Number(childMatrix.def));
    }

    const generateControlElements = ()=>{
        const filteredDevs = devices.filter((dev) => {
            return dev.id != selectedDevice;
        })

        let noDevices = false;
        const options = [];
        filteredDevs.forEach((dev)=>{
            options.push({value:dev.id, title:dev.tag});
        });

        if(options.length === 0)
        {
            options.push({value:-1, title: 'No devices', disabled: true});
            noDevices = true;
        }

        return {
            childMatrix:{def:childDevice, options:options},
            connect:{color: noDevices ? '#909090' : '#00aa00', def:false}
        };
    }

    return ( 
    <>  
        <div className="card settings-card">
            <ControlUI name="syncUI" onChangeCb={handleChange} controlElements={generateControlElements()}/>
        </div>          
    </> 
    );
}

export default SynchonisationUI;