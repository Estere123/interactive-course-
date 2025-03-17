import React, { useState } from 'react'

export default function Square({value, onSquareClick, isWinning}) {
const squareClass = `square ${isWinning? 'square-winning' : ''}`;

  return (
    
    <div>
      <button className= {squareClass} onClick={onSquareClick}>
        {value}
</button>
    </div>
  )
}
