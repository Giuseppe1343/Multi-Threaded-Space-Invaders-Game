# Multi-Threaded Space Invaders Game
> A c++ game developed to understand the basics of multi-threaded programming.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT) ![Environment: Windows](https://img.shields.io/badge/Environment-Windows-blue) 

<div align="center">
  <img src="https://github.com/Giuseppe1343/Multi-Threaded-Space-Invaders-Game/blob/main/gameplay.gif"/>
</div><be>


### Table of contents
- [Introduction](#introduction)
- [Features](#features)
- [Gameplay Overview](#gameplay-overview)
- [Dependencies](#dependencies)

  
#  Introduction

This project is a simple implementation of the classic Space Invaders game using C++14. The game features a player-controlled ship, alien invaders, UFOs, and various types of bullets. It was developed by assigning different tasks to multiple threads without using any game development engine for game development. Basically, 4 groups of threads work by communicating through mutex and flags. Input Thread, Scene (Timer) Thread, Entity Threads, Player Threads

> [!WARNING]
> Inspired by the classic Space Invaders game.

# Features

- Player-controlled ship that can move and fire bullets.
- Alien invaders arranged in a matrix.
- UFOs that appear occasionally.
- Different types of alien bullets.
- Sound effects for various game events.
- Fullscreen and windowed modes.
- High score tracking.

# Gameplay Overview

In this Space Invaders game, the player controls a ship at the bottom of the screen and must defend against waves of alien invaders. The objective is to destroy all the aliens while avoiding their bullets and preventing them from reaching the bottom of the screen. The game also features a UFO that occasionally appears and can be shot for bonus points.

## Game Elements

### Player's Ship
- **Movement**: The ship can move left and right using the arrow keys.
- **Firing**: The ship can fire bullets by pressing the spacebar.
- **Lives**: The player has a limited number of lives, which decrease when hit by alien bullets.

### Aliens
- **Formation**: Aliens are arranged in a matrix formation and move horizontally across the screen.
- **Descending**: When an alien reaches the edge of the screen, the entire formation descends one step and changes direction.
- **Firing**: Aliens periodically fire bullets towards the player's ship.

### UFO
- **Appearance**: A UFO occasionally appears at the top of the screen and moves horizontally.
- **Bonus Points**: Shooting the UFO grants bonus points.

### Bullets
- **Player's Bullets**: Fired by the player's ship to destroy aliens and the UFO.
- **Alien Bullets**: Fired by aliens to hit the player's ship.
  
### Barriers
- **Barrier**: A structure that prevents bullets from passing behind it and breaks as the bullet hits it.
  
### Game States
- **Power On**: Indicates whether the game is currently running.
- **Full Screen**: Toggles between fullscreen and windowed mode.
- **Game Over**: Indicates whether the game has ended.
- **Level**: Tracks the current level of the game.
- **High Score**: Tracks the highest score achieved.

## Controls

- **Arrow Keys**: Move the player's ship left and right.
- **Spacebar**: Fire bullets.
- **F11** - **Esc**:: Toggle fullscreen mode.
- **P**: Pause the game.

## Objective

The main objective of the game is to achieve the highest score possible by:
- Destroying all the aliens in each wave.
- Shooting the UFO for bonus points.
- Avoiding alien bullets and preventing the aliens from reaching the bottom of the screen.

## Scoring

- **Alien**: Each alien destroyed grants a certain number of points.
- **UFO**: Shooting the UFO grants bonus points, which are randomly determined.

## Game Over

The game ends when:
- The player's ship is hit by an alien bullet and no lives remain.
- The aliens reach the bottom of the screen.

The player's final score is displayed, and if it is higher than the previous high score, it is saved as the new high score.

# Dependencies

>  Windows API for handling threads and synchronization.

>  Standard C++ libraries for random number generation and atomic operations.

> [!IMPORTANT]
> [ICBYTES](https://github.com/cembaykal/ICBYTES) library for creating GUI interfaces and graphics.
