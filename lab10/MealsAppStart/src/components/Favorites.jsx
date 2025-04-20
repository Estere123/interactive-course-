
const Favorites = ({favoriteMeals}) => {
  return (
    <div className="favorites-section">
      <h3>Favorites</h3>
      <ul className="favorites">
        {favoriteMeals?.map(meal => (
          <li key={meal.idMeal}>
          <img 
          id="fav-img" 
          src={meal.strMealThumb}
          alt={meal.strMeal}/>
          <span>{meal.strMeal}</span>
          <button className="clear">
            <i className="fas fa-window-close"></i>
            </button>
            </li>
        ))}
      
      </ul>
    </div>
  )
}

export default Favorites
