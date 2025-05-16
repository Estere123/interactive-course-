import { useState } from "react";

const Search = ({onSearch}) => {
  const [searchTerm, setSearchTerm] = useState('');

  const handleSearch = () => {
    const trimmedTerm = searchTerm.trim();
    if (trimmedTerm) {
      onSearch(searchTerm);
      setSearchTerm('');
    }
  };

  return (
    <div className="row">
      <div className="search-box">
        <input 
          type="text" 
          placeholder="Search for your fav meal" 
          value={searchTerm}
          onChange={(e) => setSearchTerm(e.target.value)}
          onKeyDown={(e) => e.key === 'Enter' && handleSearch()}
        />
        <button id="search" onClick={handleSearch}>
          <i className="fas fa-search"></i>
        </button>
      </div>
    </div>
  );
}

export default Search
