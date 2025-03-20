import React, { useEffect, useState } from 'react';

const  UserProfileForm = () => {
const [name, setName] = useState("");
const [email, setEmail] = useState("");
const [password, setPassword] = useState("");
const [errors, setErrors] = useState({})

useEffect(() => {
  console.log("we have an effect")
  const errorObj = {};
  
if(name.length >0 && name.length<6)
{
    errorObj.name="Size should be more than 6";
}

},[name, email]);
  return (
    <div>
      <form>
        <div>
            <label htmlFor='name'>Name</label>
            <input 
            type='text' 
            value={name} 
            id='name'
            onChange={(e) => {
           setName(name.target.value)
            }} />
            {errors.name && <p>{errors.name}</p>}
        </div>

        <div>
            <label htmlFor='email'>Email</label>
            <input type='text' value={email} id='email' 
            
    
            onChange={(e) => {
           setEmail(email.target.value)
            }}

        
            />
        </div>
        <div>
            <label htmlFor='password'>Password</label>
            <input type='text' value={password} id='password' 
             onChange={(e) => {
                setPassword(password.target.value)
             }}
            />
        </div>

<button type='submit'>Submit</button>
      </form>
    </div>
  )

}

export default UserProfileForm


