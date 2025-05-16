import { Outlet } from 'react-router';
import Navbar from './Navbar';

const Layout = ({ children }) => {
  return (
    <div className="app-container">
      <Navbar />
      <main className="main-content">
        <Outlet/>
      </main>
    </div>
  );
};

export default Layout;