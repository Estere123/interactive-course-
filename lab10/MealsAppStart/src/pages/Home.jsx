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
  const [searchMeals, setSearchMeals] = useState([]);

  useEffect(()=> {
    const storedFavorites = JSON.parse(localStorage.getItem("favoriteMeals")) || [];
    setFavoriteMeals(storedFavorites);
    loadRandomMeal();
  }, []);
  

  const loadRandomMeal = async () => {
    const resp = await fetch(RAMDOMAPI);
    const data = await resp.json();
    let meal = data.meals[0];
    console.log(meal);
    setRandomMeal(meal);
  }
const addToFavorites = (meal) => {
  if (!favoriteMeals.some((fav) => fav.idMeal === meal.idMeal)) {
    const updateFavorites = [...favoriteMeals, meal];
    setFavoriteMeals(updateFavorites);
    localStorage.setItem("favoriteMeals", JSON.stringify(updateFavorites));
  }
};

const removeFromFavorites = (mealId) => {
  const updateFavorites = favoriteMeals.filter((meal)=> meal.idMeal !== mealId);
  setFavoriteMeals(updateFavorites);
  localStorage.setItem("favoriteMeals", JSON.stringify(updateFavorites));
};

  const getMealById = async () => {
    const resp = await fetch(MEALS_BYID_API);
    const data = await resp.json();
    let meal = data.meals[0];
    console.log(meal);
    getRandomMeal(meal);
  }

  const handleSearch = async (query) => {
    const resp = await fetch(`${Search_API}${query}`);
    const data = await resp.json();
    console.log(data);

    if (data.meals) {
      setRandomMeal(null);
      setSearchMeals(data.meals);
    } else {
      setSearchMeals([]);
    }
  };

  return (
    <div className="store">
      <Search onSearch={handleSearch} />

      <Favorites 
      favoriteMeals={favoriteMeals}
      removeFromFavorites = {removeFromFavorites}
      />

      <div className="meals" id="meals">
        {searchMeals.length >0 ?(
          searchMeals.map((meal) => (
        <MealCard
        key={meal.idMeal}
        mealData={meal}
        addToFavorites={addToFavorites}
        />
          ))
        
        ) : randomMeal ? (
        <MealCard 
        mealData={randomMeal}
          isRandom={true}
          addToFavorites={addToFavorites}
       />
        ) : (
          <p>No meals found</p>
      )}
        
      </div>

    </div>
  )
}

export default Home
