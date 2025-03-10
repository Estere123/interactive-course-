import { useState } from 'react'
import reactLogo from './assets/react.svg'
import viteLogo from '/vite.svg'
import './App.css'
import Avatars from './Components/Avatars';
import PollButton from './Components/PollButton';
import Chat from './Components/Session';


function App() {
  const [count, setCount] = useState(0)

  return (
    <>
      <div>
      <h1>Friend Group</h1>
      <Avatars/>
      <PollButton/>
      <Chat/>
      
      
      </div>
   
    </>
  )
}

export default App
