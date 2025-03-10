import React from 'react';
import LauraImage from '../assets/images/laura.jpg';
import KateImage from '../assets/images/kate.jpg';
import LillyImage from '../assets/images/lilly.jpg';
import AnnaImage from '../assets/images/anna.jpeg';


export default function Avatars() {
  return (
    <div>
      <img src={LauraImage} alt='Laura' />
      <p>Laura</p>

      <img src={KateImage} alt='Kate' />
      <p>Kate</p>

      <img src={LillyImage} alt='Lilly' />
      <p>Lilly</p>

      <img src={AnnaImage} alt='Anna' />
      <p>Anna</p>

    </div>
  )
}
