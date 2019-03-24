# Asteroids

For EE/CS120B aka Embedded Systems at the University of California Riverside, students were asked to design their own custom lab project. Using everything we had learned the entire semester we were tasked to create our own product and share it to the world. This is the repo for that project.

Asteroids is a classic Arcade game made in the year 1979 by video game company Atari. For my custom lab project I decided to remake the game with several modifications to the classic game. Starring a lone ship, the game had players avoid and confront incoming obstacles such as asteroids.

### Prerequisites

Because this is an Embedded Systems course there is a lot of requirements needed before using any of the code in this project.

First of all, the project was built in C. Due to the 2 week due date of the project I admit that the code you will see here is perhaps the least optimal code you'll ever find. Sorry, most of my time during the project was spent figuring out the LED Matrix and the Joystick's wiring and analogue voltage conversion. That being said, the code may look foreign to most because it uses a Concurrent State Machine design. The tasks scheduler ensures every single component of the project works at the same time or at least a few microseconds apart.

Next on the list of required knowledge is general know how around electricity. You know high, ground, resistors and all of that jazz. If you don't know what you're doing with electricity it's best to just not risk your health.

Lastly while some of you reading this may have similar parts, the code I designed was specifically for the parts listed below on the "Built With" section so there is no gaurentee it will work with your components.

## Getting Started

Make sure to download all the software needed. As shown in the list below, I built this poject using ATMEL Studio and used it to program that ATMEGA. Set up all the wires to the ATMEGA 1284 as shown in my [diagram](https://docs.google.com/document/d/1MDJOOVsF-J3aBrfvD7mnaCqiGyzNE8X5TEhDOHQZ1gQ/edit?usp=sharing). After you program both ATMEGA chips, just turn off and on the system in order to start the game.

The ATEMEGA's EEPROM kicks in as soon as the system turns off. When you turn it back on the Highscore will be updated depending on whether or not the previous game's score surpassed the Highscore. In order to score points you just make it to each side of the LED Matrix while avoiding the incoming asteroids.

## Built With

- ATMEGA 1284 - 2 of these Microcontrollers are used in the project.
- [LED Dot Matrix](https://www.amazon.com/gp/product/B01JYHMLJC/ref=ppx_yo_dt_b_asin_title_o00_s00?ie=UTF8&psc=1)- This is the LED Dot Matrix used to display the game.
- [Joystick](https://www.amazon.com/gp/product/B01N2G0H1T/ref=ppx_yo_dt_b_asin_title_o01_s00?ie=UTF8&psc=1) - This is Arcade quality joystick used to control the game.
- [LCD](https://www.amazon.com/Arducam-Display-Controller-Character-Backlight/dp/B019D9TYMI/ref=sr_1_3?keywords=lcd+arduino&qid=1553384176&s=gateway&sr=8-3) - This LCD displays the score of the game, High Score, and a custom character.

## Demo

- Watch a demo video
  https://www.youtube.com/watch?v=eaqVpGNsSGg
