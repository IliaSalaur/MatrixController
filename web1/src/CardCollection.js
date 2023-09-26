import Card from './Card'

/**
 * 
 * @param {cards:[{cardTitle, cardImg, cardName}], onClickCb, onContextMenuCb} props 
 * @returns 
 */

const CardCollection = (props) => {
    const { cards, onClick, onContextMenu, style } = props;

    return ( 
        <div style={{...style}} className="card-collection">
            {
                cards.map((el) =>{
                    return(<Card key={el.cardName} cardName={el.cardName} cardTitle={el.cardTitle} cardImg={el.cardImg} elementAsImg={el.elementAsImg} onContextMenuCb={onContextMenu} onClickCb={onClick}/>);
                })
            }
        </div>
    );
}
 
export default CardCollection;