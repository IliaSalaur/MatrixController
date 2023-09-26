import { useState, useRef } from 'react';
import { Link, useLocation } from 'react-router-dom'
import { CSSTransition } from 'react-transition-group'

import menu from './img/menu.svg'
import close from './img/close.svg'

const Navigation = (props) => {
    const [drawerState, setDrawerState] = useState(false);
    const { drawerList, drawerTitle } = props;
    const nodeRef = useRef(null);
    const currentLocation = useLocation();

    // useEffect(() =>{
    // }, [currentLocation]);

    const handleClose = () => {
        setDrawerState(false);
    }

    const handleOpen = () => {
        setDrawerState(true)
    }

    const getCurrentLocationName = () =>{
        for(let el in drawerList)
        {
            if(drawerList[el].route === currentLocation.pathname)
                return drawerList[el].name;
        }
        return '';
    }

    return (   
        <>    
        <header className="nav-header">
            <span className="nav-menu-icon" onClick={handleOpen}><img className="bw-icon" src={menu} alt="" /></span>
            <h1 className="nav-title">{drawerTitle !== undefined ? drawerTitle : getCurrentLocationName()}</h1>
        </header>
        <CSSTransition
            in={drawerState}
            nodeRef={nodeRef}
            timeout={200}
            onEnter={() => setDrawerState(true)}
            onExited={() => setDrawerState(false)}
            classNames={{
                enter:'drawer-wrap-enter',
                enterActive:'drawer-wrap-enter-active',
                exit:'drawer-wrap-exit',
                exitActive:'drawer-wrap-exit-active'
            }}
            unmountOnExit
        >
            <div ref={nodeRef} className='drawer-wrap' onClick={handleClose}>
                <div className="drawer">
                    <span className="drawer-close-icon" onClick={handleClose}><img className="bw-icon" src={close} alt="" /></span>
                    <h2 className="drawer-title">{drawerTitle}</h2>
                    <ul className='drawer-list'>
                        {
                            drawerList.map( listOption => <li className='drawer-option' key={listOption.route}><Link className={'drawer-href'} to={listOption.route}>{listOption.name}</Link></li> )
                        }
                    </ul>
                </div>                        
            </div>     
        </CSSTransition>
        </>
    );
}
 
export default Navigation;