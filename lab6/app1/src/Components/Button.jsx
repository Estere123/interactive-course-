import React from 'react'

export default function Button(props) {
 
  
  return (
    <div>
      <button onClick={props.functionHandler}>
          count is {props.value}
        </button>

    </div>
  )
}
