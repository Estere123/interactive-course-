import { Navigate } from "react-router";

const isAuthenticated = () => {
    return localStorage.getItem('auth') === 'true';
}

export default function ProtectedRoute({children}) {
    if (!isAuthenticated()) {
        return <Navigate to={'/'} replace />
    }

    return children;
}