import React, { useState } from 'react'

export default function Square({value, onSquareClick}) {
    

  return (
    
    <div>
      <button className="square" onClick={onSquareClick}>
        {value}
</button>
    </div>
  )
}
