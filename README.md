# Cellular Automaton Physics Project
## Features
* Simple 2d physics
    * Fire physics
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
### Linux
Clone the repo:  
`git clone https://github.com/Blayung/ca-physics.git`  
`cd ca-physics`  

Install sdl2 and sdl2 image (I use pacman for example):  
`sudo pacman -S sdl2`  
`sudo pacman -S sdl2_image`  

Compile the program:  
`g++ main.cpp -lSDL2 -lSDL2_image`  

Run it:  
`./a.out`
### Windows
1. Download [this repo](https://github.com/Blayung/ca-physics/archive/refs/heads/main.zip/).
2. Install [mingw](https://sourceforge.net/projects/mingw/), and add its executables to the path.
3. Download [sdl2](https://github.com/libsdl-org/SDL/releases/) and [sdl2 image](https://github.com/libsdl-org/SDL_image/releases/), and then extract them somewhere.
4. Open cmd, and go into the extracted repo.
5. Compile the program: `g++ main.cpp -Ipath/to/sdl2/x86_64-w64-mingw32/include/ -Lpath/to/sdl2/x86_64-w64-mingw32/lib/ -Ipath/to/sdl2image/x86_64-w64-mingw32/include/ -Lpath/to/sdl2image/x86_64-w64-mingw32/lib/ -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -mwindows`
6. Run the program: `a.exe`
