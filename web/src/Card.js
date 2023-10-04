import { useRef } from "react";

const Card = (props) => {
    const {cardImg, cardTitle, cardName, onClickCb, onContextMenuCb, elementAsImg} = props;
    const checkHold = useRef(null);

    const handleTouchStart = (event) =>{
        console.log("touch start"); 
            checkHold.current = {isTouched:true, event:event};
            setTimeout(()=>{
                if(checkHold.current.isTouched == true)
                onContextMenuCb(event, cardName);
            }, 750);
    };

    const handleTouchEnd = (event) =>{
        console.log("touch end"); 
            checkHold.current = {isTouched: false, event:event}
    }

    return ( 
        <div className="card" 
        onTouchStart={handleTouchStart}
        onMouseDown={handleTouchStart}
        onTouchEnd={handleTouchEnd}
        onMouseUp={handleTouchEnd}
        // onContextMenu={(event) => {onContextMenuCb(event, cardName)}} 
        onContextMenu={(event) => {event.preventDefault()}} 
        onClick={(event)=>{onClickCb(event, cardName)}}
        >
            <p className="card-title">{cardTitle}</p>
            {elementAsImg === undefined && <img className="card-img" src={cardImg} alt="" />}
            {elementAsImg === true && <>{cardImg}</>}
        </div>
    );
}
 
export default Card;