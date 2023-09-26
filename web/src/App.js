import Navigation from './Navigation';
import { BrowserRouter as Router, Route, Routes } from 'react-router-dom';
import EffectsCollection from './EffectsCollection';
import TextSequenceCollection from './TextTemplates/TextSequenceColletion';
import Settings from './Settings';
import { DevicesContext } from './DevicesContext';
import DeviceSelector from './DeviceSelector';
import { useState, useEffect } from 'react';
import * as requests from "./requests";
import SynchonisationUI from './SyncUI';

const Menu = [
    {name:'Main Page', route: '/'},
    {name:'Effects', route: '/effects'},
    {name:'Text', route: '/text'},
    {name:'Clock', route: '/clock'},        
    {name:'Entertainment', route: '/entertainment'},    
    {name:'Timer', route: '/timer'},
    {name:'Weather', route: '/weather'},
    {name:'Settings', route: '/settings'},
    {name:'Synchonisation', route: '/sync'}
];

function deepEqual(x, y) {
    return (x && y && typeof x === 'object' && typeof y === 'object') ?
      (Object.keys(x).length === Object.keys(y).length) &&
        Object.keys(x).reduce(function(isEqual, key) {
          return isEqual && deepEqual(x[key], y[key]);
        }, true) : (x === y);  
}

const App = (props) => {
    const [selectedDevice, setDevice] = useState(undefined);
    // const [devices, setDevices] = useState([{id:123,h:16,w:16,tag:'hi', ip:'192.168.0.2'}]);
    const [devices, setDevices] = useState([]);

    const handleSelect = (e) =>{
        setDevice(Number(e.target.value));
    }

    useEffect(()=>{
        let devs = [];

        const updateDeviceList = async () =>{
            let url = 'http://' + requests.LOCATION + '/getDevices';

            try{
                const res = await fetch(url);
                const json = await res.json();

                // console.log('newData', json, devs);
                if(!deepEqual(json, devs))
                {
                    console.log('Objects appeared to be different', json, devs);
                    devs = [...json];
                    setDevices([...json]);
                }              
            }
            catch(e){
                console.log(e);
            }
        }

        updateDeviceList();

        const fetchInterval = setInterval(()=>{
            console.log();
            updateDeviceList();            
        }, 10000);

        return () => {
            clearInterval(fetchInterval);
        };
    }, []);  

    // selectedDevice state update decoupled from the first useEffect for being sure, that the accessed selectedDevice value is valid (loaded) 
    useEffect(()=>{
        if(devices.length === 0) return;
        if(selectedDevice == null)
        {
            setDevice(devices[0].id);
        }
    }, [devices]);

    return ( 
        <Router>            
                <DevicesContext.Provider value={{selectedDevice:selectedDevice, devices:devices}}>
                    <Navigation drawerList={Menu} locations={Menu}/>
                    <DeviceSelector handleSelect={handleSelect} devices={devices} selectedDevice={selectedDevice}/>
                    <Routes>
                        <Route exact path='/' element={<h1>Hello</h1>} />
                        <Route path='/effects' element={<EffectsCollection/>} /> 
                        <Route exact path='/text' element={<TextSequenceCollection key={selectedDevice}/>}/>
                        <Route exact path='/settings' element={<Settings/>}/>
                        <Route exact path='/clock' element={<h1>In development</h1>}/>
                        <Route exact path='/timer' element={<h1>In development</h1>}/>
                        <Route exact path='/weather' element={<h1>In development</h1>}/>
                        <Route exact path='/entertainment' element={<h1>In development</h1>}/>
                        <Route exact path='/sync' element={<SynchonisationUI/>}/>

                    </Routes>    
                </DevicesContext.Provider>
        </Router>
    );
    
}

export default App;