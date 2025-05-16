import { Link } from 'react-router'

const Favorites = ({favoriteMeals, removeFavorite}) => {
  return (
    <div className="favorites-section">
      <h3>Favorites</h3>
      <ul className="favorites">
        {favoriteMeals.map(meal => (
          <li key={meal.idMeal}>
            <Link to={`/meal/${meal.idMeal}`}>
            <img 
              id="fav-img" 
              src={meal.strMealThumb} 
              alt={meal.strMeal}
            />
            </Link>
            <span>{meal.strMeal}</span>
            <button className="clear" onClick={() => removeFavorite(meal.idMeal)}>
              <i className="fas fa-window-close"></i>
            </button>
          </li>
        ))}
      </ul>
    </div>
  )
}

export default Favorites
