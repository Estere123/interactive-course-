import Search from "../components/Search";
import Favorites from "../components/Favorites";
import MealCard from "../components/MealCard";
import { useEffect, useState } from "react";

const RAMDOMAPI = import.meta.env.VITE_RANDOM_MEAL_API;
const MEALS_BYID_API = import.meta.env.VITE_MEAL_BYID_API;
const Search_API = import.meta.env.VITE_SEARCH_MEAL_API;

const Home = () => {
  const [randomMeal, setRandomMeal] = useState(null);
  const [favoriteMeals, setFavoriteMeals] = useState([]);
  const [favoriteMealsIDs, setFavoriteMealsIDs] = useState([]);
  useEffect(() => {
    loadRandomMeal()
  }, [])

  const loadRandomMeal = async () => {
    const resp = await fetch(RAMDOMAPI);
    const data = await resp.json();
    let meal = data.meals[0];
    console.log(meal);
    setRandomMeal(meal);
  }

  const getMealById = async () => {
    const resp = await fetch(MEALS_BYID_API);
    const data = await resp.json();
    let meal = data.meals[0];
    console.log(meal);
    getRandomMeal(meal);
  }

  return (
    <div className="store">
      <Search />

      <Favorites />

      <div className="meals" id="meals">
        {randomMeal && (
        <MealCard 
        mealData={randomMeal}
          isRandom={true}
       />
        )}
        
      </div>

    </div>
  )
}

export default Home
