import { useContext, useEffect, useRef, useState } from "react";
import CardCollection from "./CardCollection";
import ControlUI from "./InputElements/ControlUI";
import { CSSTransition } from "react-transition-group";
import * as requests from "./requests"
import { DevicesContext } from "./DevicesContext";

// Images
import fire from './img/fire.svg'
import matrix from './img/matrix.svg'
import rainbow from './img/rainbow.svg'
import plasmawaves from './img/plasmawaves.svg'
import sparkles from './img/sparkles.svg'
import waterfall from './img/waterfall.svg'
import crazybees from './img/crazybees.svg'
import snowfall from './img/snowfall.svg'


export const EffectCards = [
    { cardName: 'Fire', cardImg: fire, cardTitle: 'Fire' },
    { cardName: 'Matrix', cardImg: matrix, cardTitle: 'Matrix' },
    { cardName: 'Rainbow', cardImg: rainbow, cardTitle: 'Rainbow' },
    { cardName: 'PlasmaWaves', cardImg: plasmawaves, cardTitle: 'Plasma Waves' },
    { cardName: 'CrazyBees', cardImg: crazybees, cardTitle: 'Crazy Bees' },
    { cardName: 'Sparkles', cardImg: sparkles, cardTitle: 'Sparkles' },
    { cardName: 'Waterfall', cardImg: waterfall, cardTitle: 'Waterfall' },
    { cardName: 'SnowFall', cardImg: snowfall, cardTitle: 'SnowFall' },
];

export const Effects = {
    Fire: { palette: { def: 0, options: [{ title: 'Default', value: 0 }, { title: 'Blue', value: 1 }, { title: 'Green', value: 2 }] } },
    Matrix: { density: { min: 10, def: 40, max: 250 } },
    Rainbow: { scale: { min: 1, max: 50, def: 20 }, speed: { min: 1, max: 40, def: 5 }, direction: { def: 0, options: [{ title: 'Horizontal', value: 0 }, { title: 'Vertical', value: 1 }, { title: 'Diagonal', value: 2 }] } },
    PlasmaWaves: { speed: { min: 20, max: 250, def: 40 } },
    CrazyBees: { scale: { min: 2, max: 10, def: 3 } },
    Sparkles: { scale: { min: 1, max: 30, def: 10 }, speed: { min: 10, max: 150, def: 50 } },
    Waterfall: {
        hue: { min: 1, def: 100, max: 255 }, cooling: { min: 10, def: 20, max: 150 }, sparking: { min: 20, def: 35, max: 200 }, mode: {
            def: 0, options: [
                { title: 'Simple', value: 0 },
                { title: 'Colors', value: 1 },
                { title: 'Fire', value: 2 },
                { title: 'Cold Fire', value: 3 }
            ]
        }
    },
    SnowFall:{scale:{min:5, def: 10, max: 255}, speed:{min:2, def: 40, max: 70}}
};

const EffectsCollection = (props) => {
    // Name
    const {selectedDevice, devices} = useContext(DevicesContext);
    const [effectToShow, setEffectToShow] = useState(null);
    const [efProps, setEfProps] = useState({[selectedDevice]:{...Effects}});
    const nodeRef = useRef(null);    

    const getDevice = ()=>{
        const newDevs = devices.filter(dev => dev.id === selectedDevice);
        return newDevs[0];
    }

    useEffect(()=>{    
        if(selectedDevice in efProps === false)
        {
            console.log("Add props", selectedDevice);   
            // setEfProps({...efProps, [selectedDevice]:{...JSON.parse(JSON.stringify(Effects))}});//old
            setEfProps({...efProps, [selectedDevice]:{...structuredClone(Effects)}});
        }

    }, [selectedDevice]);

    const handleShowEffectUI = (event, effectName) => {
        event.preventDefault();
        setEffectToShow(effectName);
    }

    const handlePropertiesChange = (newObj) => {
        // let newProps = {
        //     ...efProps,
        //     [selectedDevice]:{
        //         ...efProps[selectedDevice],
        //         [effectToShow]:newObj
        //     }
        // };

        // let newProps = JSON.parse(JSON.stringify(efProps)); //old
        let newProps = structuredClone(efProps);
        efProps[selectedDevice][effectToShow] = {...newObj};
        // newProps[effectToShow] = newObj;
        console.log(newProps);

        requests.setEffect(effectToShow, newProps[selectedDevice][effectToShow], getDevice());
        setEfProps(newProps);
    }

    const handleClick = (e, effectName) => {
        requests.setEffect(effectName, efProps[selectedDevice][effectName], getDevice());
    }

    const handleCloseEffectUI = (e) => {
        if (e.target.className === 'effect-ui-wrapper') setEffectToShow(null);
    }

    return (
        <>
            <CardCollection cards={EffectCards} onContextMenu={(e, effectName) => { handleShowEffectUI(e, effectName) }} onClick={(e, effectName) => { handleClick(e, effectName) }} />
            <CSSTransition
                in={effectToShow !== null}
                nodeRef={nodeRef}
                timeout={200}
                // onEnter={() => setDrawerState(true)}
                // onExited={() => setDrawerState(false)}
                classNames={{
                    enter: 'effect-ui-enter',
                    enterActive: 'effect-ui-enter-active',
                    exit: 'effect-ui-exit',
                    exitActive: 'effect-ui-exit-active'
                }}
                unmountOnExit
            >
                <div ref={nodeRef} className="effect-ui-wrapper" onClick={handleCloseEffectUI}>
                    <div className="effect-ui">
                        {effectToShow !== null && <ControlUI key={'ll' + props.selectedDevice} controlElements={efProps[selectedDevice][effectToShow]} onChangeCb={handlePropertiesChange} />}
                    </div>
                </div>
            </CSSTransition>
        </>
    );
}

export default EffectsCollection;