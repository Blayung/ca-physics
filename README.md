## Features
* Simple 2d physics
    * Fire spreading physics
    * Gases physics
    * Liquid physics
    * Powders physics
* Particles: stone, wood, water, oil, sand, gunpowder, fire, lava, steam, smoke
## Controls
**Scroll ->** Choose a particle  
**LMB ->** Place a particle  
**RMB ->** Remove a particle  
**R ->** Reset the grid  
**Q/Escape ->** Quit  
## Compiling
**Clone the repo:**
`git clone https://github.com/Blayung/ca-physics.git`
`cd ca-physics`

**Install sdl2 and sdl2 image (using pacman for example):**
`sudo pacman -S sdl2`
`sudo pacman -S sdl2_image`

**Compile the program:**
`g++ main.cpp -lSDL2 -lSDL2_image`

**Run the program:**
`./a.out`
