
import { useEffect, useState } from 'react';

const MealDetails = ({id}) => {
  const [meal, setMeal] = useState(null);
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    const fetchMealDetails = async () => {
      try {
        const response = await fetch(`https://www.themealdb.com/api/json/v1/1/lookup.php?i=${id}`);
        const data = await response.json();
        setMeal(data.meals[0]);
      } catch (error) {
        console.error('Error fetching meal details:', error);
      } finally {
        setLoading(false);
      }
    };

    fetchMealDetails();
  });

  if (loading) return <div className="loading">Loading meal details...</div>;
  if (!meal) return <div>Meal not found</div>;

  // Extract ingredients and measures
  const ingredients = [];
  for (let i = 1; i <= 20; i++) {
    if (meal[`strIngredient${i}`]) {
      ingredients.push({
        name: meal[`strIngredient${i}`],
        measure: meal[`strMeasure${i}`]
      });
    }
  }

  return (
    <div className="meal-details-container">
            
      <div className="meal-details">
        <div className="meal-image">
          <img src={meal.strMealThumb} alt={meal.strMeal} />
          <h1>{meal.strMeal}</h1>
          {meal.strArea && <p className="meal-area">{meal.strArea}</p>}
        </div>
        
        <div className="meal-info">
          <div className="ingredients">
            <h2>Ingredients</h2>
            <ul>
              {ingredients.map((item, index) => (
                <li key={index}>
                  <span className="ingredient-name">{item.name}</span>
                  <span className="ingredient-measure">{item.measure}</span>
                </li>
              ))}
            </ul>
          </div>
          
          <div className="instructions">
            <h2>Instructions</h2>
            <p>{meal.strInstructions}</p>
          </div>
        </div>
      </div>
    </div>
  );
};

export default MealDetails;