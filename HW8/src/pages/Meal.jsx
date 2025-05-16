import { Link, useParams } from 'react-router';
import MealDetails from '../components/MealDetails';

const Meal = () => {
  const {id} = useParams();
  return (
    <div className="meal-page">
      <Link to="/">
      <span>Return to Home</span>
      </Link>
      
      <MealDetails id={id}/>
    </div>
  );
};

export default Meal;