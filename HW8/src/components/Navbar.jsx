import { Link } from 'react-router';

const Navbar = () => {
  return (
    <nav className="navbar">
      <ul className="nav-list">
        <li className="nav-item">
          <Link to="/" className="nav-link">Home</Link>
        </li>
        <li className="nav-item">
          <Link to="/contacts" className="nav-link">Contacts</Link>
        </li>
        <li className="nav-item">
          <Link to="/about" className="nav-link">About</Link>
        </li>
        <li className="nav-item">
          <Link to="/admin" className="nav-link">Admin</Link>
        </li>
      </ul>
    </nav>
  );
};

export default Navbar;