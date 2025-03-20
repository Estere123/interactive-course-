import { useState } from 'react'
import reactLogo from './assets/react.svg'
import viteLogo from '/vite.svg'
import './App.css'
import Button from './Components/Button'
import UserProfileForm from './Components/UserProfileForm'


  function App() {
    const [count, setCount] = useState(0)

  const handleClick = () => {
    setCount((count) => count + 1)
  }

  return (
    <>
     <UserProfileForm/>
    </>
  )
}

export default App
