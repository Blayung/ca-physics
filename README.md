# Cellular Automaton Physics Project
# DISLAIMER: I'M CURRENTLY REWRITING THIS PROJECT IN RUST, THE C++ VERSION WON'T GET ANY MORE SUPPORT. YOU CAN CHECK OUT THE RUST DEVELOPMENT BRANCH OVER [HERE](https://github.com/Blayung/ca-physics/tree/rust-dev).
## Features
* Simple 2d physics
    * Gases physics
    * Liquid physics
    * Powders physics
* Particles: stone, wood, water, oil, sand, gunpowder, fire, lava, steam, smoke, flammable gas
* Time control, custom grid and particle sizes.
## Controls
**Scroll ->** Choose a particle  
**LMB ->** Place a particle  
**RMB ->** Remove a particle  
**R ->** Reset the grid  
**Q/Escape ->** Quit  
**Left/Right arrow ->** Change speed  
## Compiling (assuming you're on linux)
Clone the repo:  
`git clone https://github.com/Blayung/ca-physics.git`  
`cd ca-physics`  

Install sdl2 and sdl2 ttf (I use pacman for example):  
`sudo pacman -S sdl2`  
`sudo pacman -S sdl2_ttf`  

Compile the program:  
`g++ main.cpp -lSDL2 -lSDL2_ttf`  

Run it:  
`./a.out`
