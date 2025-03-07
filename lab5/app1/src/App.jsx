import { useState } from 'react'
import reactLogo from './assets/react.svg'
import viteLogo from '/vite.svg'
import './App.css'
import MyButton from './Components/MyButton'
import Information from './Components/information'
import Job from './Components/Job'
import Education from './Components/Education'


function App() {
  
let content;
let flag = true;
if(flag) {
content = <Job/>
}
else {
content = <Education/>
}
  return (
    <div> 
  <h1>Hello, I am Estere</h1>
<MyButton/>
<Information home="Estonia" study="HCI" like="swimming"/>

{content}
<h3>---------------new-----------</h3>
{flag? (<Job/>):(<Education/>)}
<h3>---------------Additional-----------</h3>
{flag && <Education/>}
  </div>
  )
}

export default App
