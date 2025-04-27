import { useState } from "react";

const Search = ({onSearch}) => {
  const [searchTerm, setSearchTerm] = useState ("");

  const handleSubmit = (e) => {
    e.preventDefault();
    if (searchTerm.trim() !== "") {
      onSearch(searchTerm);
    }
  };
  
  return (
    <div className="row">
      <form onSubmit={handleSubmit}>
        <input 
        type="text"
        placeholder="Search meals..."
        value={searchTerm}
        onChange={(e) => setSearchTerm(e.target.value)}
        />
        <button type="submit">Search</button>
      </form>
    </div>
  );
}

export default Search
