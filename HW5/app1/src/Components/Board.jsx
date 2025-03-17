import React, { useState } from 'react'
import Square from './Square'

export default function Board({xIsNext, squares, onPlay}) {
  const [winningLine, setWinningLine] = useState(null);

  function handleClick(i) {
    if ( calculateWinner(squares) || squares[i]) {
      return;
    }
  const nextSquares = squares.slice();
  if (xIsNext) {
  nextSquares[i] = 'X';
} else {
  nextSquares[i] = 'O';
}
onPlay(nextSquares);
const winnerLine = calculateWinner(nextSquares);
if (winnerLine) {
  setWinningLine(winnerLine)
} else {
  setWinningLine(null);
}
  
}

const winner = calculateWinner(squares);
let status;
if (winner) {
  status = 'Winner: ' + squares[winner[0]];
} else if (squares.every((square)=> square)) {
  status = 'Draw';
} else {
  status = 'Next player: ' + (xIsNext ? 'X': 'O');
}

const boardRows = [];
for (let row =0; row <3; row++) {
  const squaresInRow = [];
    for (let col = 0; col <3; col++) {
      const squareIndex = row*3 +col;
      squaresInRow.push (
       <Square
       key={squareIndex} 
       value={squares[squareIndex]}
       onSquareClick={() =>  handleClick(squareIndex)}
       isWinning={winningLine && winningLine?.includes(squareIndex)}
       />
      )
    }
    boardRows.push(
      <div className='board-row' key={row}>
        {squaresInRow}
      </div>
    )
  
}

  return (
    <div>
    <div className="status">{status}</div>
    {boardRows}
    </div>
     
    
  )
}
function calculateWinner(squares) {
  const lines = [
      [0,1,2],
      [3,4,5],
      [6,7,8],
      [0,3,6],
      [1,4,7],
      [2,5,8],
      [0,4,8],
      [2,4,6]
  ];

  for (let i=0; i<lines.length; i++) {
      const [a,b,c] = lines[i];
      if (squares[a] && squares[a] === squares[b] && squares[a] === squares[c]) {
          return lines[i];
      }
  }
return null;
}