import React from 'react';
import LauraImage from '../assets/images/laura.jpg';
import KateImage from '../assets/images/kate.jpg';
import LillyImage from '../assets/images/lilly.jpg';
import AnnaImage from '../assets/images/anna.jpeg';
import './Avatars.css';

export default function Avatars() {
  return (
    <div className='avatars-container'>
      <div className='avatar'>
        <img src={LauraImage} alt='Laura' />
        <p>Laura</p>
      </div>

      <div className='avatar'>
        <img src={KateImage} alt='Kate' />
        <p>Kate</p>
      </div>

      <div className='avatar'>
        <img src={LillyImage} alt='Lilly' />
        <p>Lilly</p>
      </div>

      <div className='avatar'>
        <img src={AnnaImage} alt='Anna' />
        <p>Anna</p>
      </div>
    </div>
  )
}
