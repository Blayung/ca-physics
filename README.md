# Cellular Automaton Physics Project
## THIS IS THE RUST DEVELOPMENT BRANCH, IF YOU WANT THE STABLE C++ VERSION OF THIS PROGRAM, PLEASE SWITCH TO THE MAIN BRANCH!
## What is this?
It is a simple 2d cellular automaton physics simulator written in rust. That program is based on an another version of the project, that I wrote before in c++. You can check out the legacy c++ version over there: [https://github.com/Blayung/ca-physics/tree/cpp-legacy-ver](https://github.com/Blayung/ca-physics/tree/cpp-legacy-ver)
## Features
- Simple 2d physics
    - Powder physics
    - Liquid physics
    - Gases physics
    - ~~Burning, evaporation and lava turning into stone~~ <- todo in the rust version, already implemented in c++.
    - ~~Time speed control~~ <- todo in the rust version, already implemented in c++, altough I'm planning for it to be improved (independent of fps) in rust.
    - ~~Brush size control~~ <- todo
- Particles/materials
    - Solids
        - Stone
        - Wood
    - Powders
        - Sand
        - Coal
    - Fluids
        - Water
        - Oil
        - Lava
    - Gases
        - Smoke
        - Steam
        - Flammable gas
    - Other
        - Air (technically a particle, but not really)
        - Fire
## Controls
**LMB ->** Place particles  
**RMB ->** Remove particles  
  
**Scroll ->** Choose the particle to draw with  
**Grave and 1234567890 ->** Choose the particle to draw with  
  
~~**Up/Down arrow ->** Change the brush's size~~ <- todo  
~~**Alt+Scroll ->** Change the brush's size~~ <- todo  
  
~~**Left/Right arrow ->** Time speed control~~ <- todo in the rust version, already implemented in c++, altough I'm planning for it to be improved (independent of fps) in rust.  
~~**Ctrl+Scroll ->** Time speed control~~ <- todo in the rust version, already implemented in c++, altough I'm planning for it to be improved (independent of fps) in rust.  
  
**Q/Escape ->** Quit  
## Todo list
- Make a menu at the bottom of the screen showing the current particle, time speed, brush's size and some other misc info.
- Add time control independent of fps.
- Add a secondary window popping out before the main window asking for the grid's size, cell's size and max fps.
- Add the brush's size control.
## How to compile? (assuming you're on linux)
0. Make sure you have git and cargo installed! If not, check these websites out: [https://git-scm.com/downloads](https://git-scm.com/downloads), [https://rustup.rs](https://rustup.rs)
1. `git clone -b rust-dev --single-branch https://github.com/Blayung/ca-physics`
2. `cd ca-physics`
3. `cargo build --release`
4. Now the executable should be here: `./target/release/ca-physics`
