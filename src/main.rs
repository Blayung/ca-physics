extern crate sdl2;

use sdl2::event::Event;
use sdl2::keyboard::Keycode;
use sdl2::mouse::MouseState;
use sdl2::pixels::Color;
use sdl2::rect::Rect;

const AIR:u32 = 0;
const STONE:u32 = 1;
const WOOD:u32 = 2;
const SAND:u32 = 3;
const COAL:u32 = 4;
const WATER:u32 = 5;
const OIL:u32 = 6;
const LAVA:u32 = 7;
const FIRE:u32 = 8;
const SMOKE:u32 = 9;
const STEAM:u32 = 10;
const FLAMMABLE_GAS:u32 = 11;

const MATERIAL_COLORS: [Color;12] = [
    Color::RGB(0, 0, 0), // AIR
    Color::RGB(90, 90, 90), // STONE
    Color::RGB(150, 130, 40), // WOOD
    Color::RGB(250, 230, 40), // SAND
    Color::RGB(40, 40, 40), // COAL
    Color::RGB(20, 40, 230), // WATER
    Color::RGB(120, 70, 10), // OIL
    Color::RGB(255, 30, 0), // LAVA
    Color::RGB(255, 100, 0), // FIRE
    Color::RGB(50, 50, 50), // SMOKE
    Color::RGB(220, 220, 220), //STEAM
    Color::RGB(20, 230, 140) // FLAMMABLE_GAS
];

struct Particle {
    pub should_move: bool,
    pub particle_type: u32,
    pub sdl_rect: Rect
}

pub fn main() {
    // TODO: Get the settings from the user at runtime
    let grid_x_size:u32=64;
    let grid_y_size:u32=64;
    let cell_size:u32=10;

    // The grid
    let mut grid: std::vec::Vec<std::vec::Vec<Particle>>=std::vec::Vec::new();
       
    let mut x:u32=0;
    loop {
        grid.push(std::vec::Vec::new());

        let mut y:u32=0;
        loop {
            grid[x as usize].push(
                Particle {
                    should_move: true,
                    particle_type: AIR,
                    sdl_rect: Rect::new((x * cell_size) as i32, (y * cell_size) as i32, cell_size, cell_size)
                }
            );

            y+=1;
            if y>grid_y_size {
                break;
            }
        }

        x+=1;
        if x>grid_x_size {
            break;
        }
    }

    // Misc vars
    let mut mouse_state: MouseState;

    // SDL Vars
    let sdl_context = sdl2::init().unwrap();
    let video_subsystem = sdl_context.video().unwrap();
    let window = video_subsystem.window("Physics simulation", grid_y_size * cell_size, grid_x_size * cell_size).position_centered().opengl().build().unwrap();
    let mut canvas = window.into_canvas().build().unwrap();
    let mut event_pump = sdl_context.event_pump().unwrap();

    // Main loop
    'main_loop: loop {
        // Events
        for event in event_pump.poll_iter() {
            match event {
                Event::Quit {..} | Event::KeyDown { keycode: Some(Keycode::Escape), .. } | Event::KeyDown { keycode: Some(Keycode::Q), .. } => break 'main_loop,
                _ => {}
            }
        }

        mouse_state=event_pump.mouse_state();
        if mouse_state.left() {
            let x: usize=(mouse_state.x()/cell_size as i32) as usize;
            let y: usize=(mouse_state.y()/cell_size as i32) as usize;
            if x>=0 && x<=grid_x_size as usize && y>=0 && y<=grid_y_size as usize{
                grid[x][y].particle_type=SMOKE;
            }
        } else if mouse_state.right() {
            let x: usize=(mouse_state.x()/cell_size as i32) as usize;
            let y: usize=(mouse_state.y()/cell_size as i32) as usize;
            if x>=0 && x<grid_x_size as usize && y>=0 && y<grid_y_size as usize{
                grid[x][y].particle_type=AIR;
            }
        }
        
        // Misc stuff happening every frame
        for x in &mut grid {
            for mut y in x {
                y.should_move=true;
            }
        }

        // Drawing to the screen
        canvas.set_draw_color(Color::RGB(0, 0, 0));
        canvas.clear();

        for x in &grid {
            for y in x {
                if y.particle_type != AIR {
                    canvas.set_draw_color(MATERIAL_COLORS[y.particle_type as usize]);
                    canvas.fill_rect(y.sdl_rect).unwrap();
                }
            }
        }

        canvas.present();

        // FPS Limit
        std::thread::sleep(std::time::Duration::new(0, 1_000_000_000u32 / 10));
    }
}
