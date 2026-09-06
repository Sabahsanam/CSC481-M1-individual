# CSC481-581Project (Individual Fork — Sabah Afroz)

The Game Engine Construction Project is designed to help you gain comprehensive, hands-on experience in the design, development, integration, and testing of a functional game engine. You will also implement individual games using your team engine to showcase system functionalities. This project allows you to develop and demonstrate competencies in game engine architecture, system implementation, and problem-solving as applied to real-world game development scenarios.

This is my individual fork for the Milestone 1 individual submission, built on top of Team 8's shared engine.

## Team 8
Sabah Afroz, safroz
Mia Glenn, mglenn2
Tram Nguyen, tnnguy22

## Setup Instructions

Follow these steps to clone, build, and run the project on your machine.

### 1. Clone the repository

```
git clone https://github.com/Sabahsanam/CSC481-M1-individual.git
```

### 2. Install SDL3

SDL3 is the latest major release. You'll need both SDL3 and `SDL3_image` (the sprite sheets are PNG format).

**On macOS (with Homebrew):**
```
brew install sdl3 sdl3_image
```

**On Ubuntu/Debian:**
```
sudo apt install libsdl3-dev libsdl3-image-dev
```

**On Arch Linux:**
```
sudo pacman -S sdl3 sdl3_image
```

**On Windows:**
Use [MSYS2](https://www.msys2.org/) or [vcpkg](https://vcpkg.io/) for easiest installation.
```
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-SDL3 mingw-w64-x86_64-SDL3_image
```
Then launch the MinGW64 shell and continue.

### 3. Build the project

```
mkdir build && cd build
cmake ..
make
```

Ensure `SDL3_DIR` is correctly set if CMake cannot find SDL.

This builds two separate programs:
- `main` — the shared team engine demo
- `sabah_game` — my individual Milestone 1 game

### 4. Run the team demo

```
./main
```

### 5. Run my individual game

```
./sabah_game
```

## My Individual Game

A princess-themed platformer built on the shared engine (`Entity`, `Physics`, `Input`, `Collision`, `Scaling` — all unchanged from the team version). Adds `Camera` and `WorldGenerator` for an endless, scrolling, chunk-generated level.

**Controls:**
- `A` / `D` — move left / right
- `Shift` + `A`/`D` — run
- `W` — jump
- `T` — toggle pixel/proportional scaling
- `ESC` — quit
- On Game Over: `R` — restart, `Q` — quit

**Features demonstrated:**
- Static obstacles (crates, totems)
- Controllable player (princess) with sprite-sheet walk animation
- Automatically moving/patrolling enemies
- Configurable gravity (player only)
- Input entirely through `Input::isKeyPressed()`, no raw SDL key events
- Multiple collision responses: obstacles block movement, platforms support standing, coins are collected, enemies/hazards end the run
- Pixel/proportional scaling toggle
- Side-scrolling camera and endless chunk-based world generation

See `Sabah_Milestone1_Writeup.docx` for the full design writeup.