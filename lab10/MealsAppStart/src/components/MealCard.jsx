import React from 'react'

const MealCard = ({mealData, isRandom = false, addToFavorites}) => {
  return (
    <div className="meal">
      <div className="meal-header">
                        {isRandom && <span className="random">Meal of the Day</span>}
                        <img src={mealData.strMealThumb} alt=""/>
                    </div>
                    <div className="meal-body">
                        <h3>{mealData.strMeal}</h3>
                        <button className="fav-btn"
                        onClick = {() => addToFavorites(mealData)}
                        >
                            <i className="fas fa-heart"></i>
                            
                        </button>
                    </div>
    </div>
  )
}

export default MealCard
