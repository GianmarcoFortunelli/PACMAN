# Pac-Man with A* AI on LandTiger Board

## 📌 Overview
A custom implementation of **Pac-Man** for the **LandTiger LPC1768** board using **Keil µVision**.  
Developed in two stages as part of the *Computer Architecture* course.

- **Stage 1**: Core Pac-Man gameplay (no ghosts), pills, scoring, lives, countdown timer, and pause function.
- **Stage 2**: AI-controlled ghost using **A\*** pathfinding, chase and frightened modes, sound effects, and CAN-based score/lives/time communication.

---

## 🕹️ Features
- **Collectibles**: 240 standard pills (+10 pts) and 6 randomly placed power pills (+50 pts).
- **Movement**: Continuous joystick control with central teleport for wrap-around movement.
- **Lives**: +1 extra life every 1000 points.
- **Pause/Resume**: Triggered via the **INT0** button.
- **Blinky Ghost AI**:
  - **Chase Mode**: Finds the shortest path to Pac-Man via A\*.
  - **Frightened Mode**: Runs away for 10 seconds after Pac-Man eats a power pill.
  - **Respawn**: Returns after 3 seconds if eaten.
- **Audio**: Background music and sound effects.
- **CAN Loopback**: Sends score, remaining lives, and time left in a 32-bit message format.

---

## 🔧 Requirements
- **Hardware**: LandTiger LPC1768 development board
- **Software**: Keil µVision IDE
