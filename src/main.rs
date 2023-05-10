extern crate sdl2;
extern crate rand;

use sdl2::event::Event;
use sdl2::keyboard::Keycode;
use sdl2::mouse::MouseButton;
use sdl2::pixels::Color;
use sdl2::rect::Rect;

//use rand::Rng;

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

const FPS_LIMIT:u32 = 10;

struct Particle {
    pub should_move: bool,
    pub particle_type: u32,
    pub sdl_rect: Rect
}

pub fn main() {
    // Getting the settings through terminal
    let mut stdinBuffer: String;
    let stdin=std::io::stdin();

    let grid_x_size: u32;
    loop {
        print!("Input the grid x size: ");
        stdinBuffer=String::new();
        let input=stdin.read_line(&mut stdinBuffer);
        if input.is_ok() {
            let inputAsInt=input.unwrap().parse::<u32>();
            if inputAsInt.is_ok() {
                grid_x_size=inputAsInt.unwrap();
                if grid_x_size > 4 { 
                    break;
                }
            }
        }
        println!("You must input an integer higher or equal to 5!");
    }

    let grid_y_size: u32;
    loop {
        print!("Input the grid y size: ");
        stdinBuffer=String::new();
        let input=stdin.read_line(&mut stdinBuffer);
        if input.is_ok() {
            let inputAsInt=input.unwrap().parse::<u32>();
            if inputAsInt.is_ok() {
                grid_y_size=inputAsInt.unwrap();
                if grid_y_size > 4 { 
                    break;
                }
            }
        }
        println!("You must input an integer higher or equal to 5!");
    }

    let cell_size: u32;
    loop {
        print!("Input the cell size: ");
        stdinBuffer=String::new();
        let input=stdin.read_line(&mut stdinBuffer);
        if input.is_ok() {
            let inputAsInt=input.unwrap().parse::<u32>();
            if inputAsInt.is_ok() {
                cell_size=inputAsInt.unwrap();
                if cell_size > 4 { 
                    break;
                }
            }
        }
        println!("You must input an integer higher or equal to 5!");
    }

    // Misc variables
    //let mut rng=rand::thread_rng();

    //let mut fps_limit=10;
    let mut is_lmb_pressed=false;
    let mut is_rmb_pressed=false;

    // The grid
    let mut grid: [[Particle; grid_y_size]; grid_x_size];
    let mut x=0;
    loop {
        let mut y=0;
        loop {
            grid[x][y].particle_type=LAVA;
            grid[x][y].sdl_rect=Rect::new((x * cell_size) as i32, (y * cell_size) as i32, cell_size, cell_size);
            y+=1;
            if y>grid.grid_y_size {
                break;
            }
        }
        x+=1;
        if x>grid.grid_x_size {
            break;
        }
    }

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
                Event::MouseButtonDown { mouse_btn: MouseButton::Left, .. } => is_lmb_pressed=false,
                Event::MouseButtonUp { mouse_btn: MouseButton::Left, .. } => is_lmb_pressed=true,
                Event::MouseButtonDown { mouse_btn: MouseButton::Right, .. } => is_rmb_pressed=false,
                Event::MouseButtonUp { mouse_btn: MouseButton::Right, .. } => is_rmb_pressed=true,
                _ => {}
            }
        }
        println!("R:{}, L:{}",is_lmb_pressed,is_rmb_pressed);
        
        // Misc stuff happening every frame
        for i in grid {
            for j in i {
                j.should_move=true;
            }
        }

        // Drawing to the screen
        canvas.set_draw_color(Color::RGB(0, 0, 0));
        canvas.clear();

        for i in grid {
            for j in grid {
                if j.particle_type != AIR {
                    canvas.set_draw_color(MATERIAL_COLORS[j.particle_type]);
                    canvas.fill_rect(j.sdl_rect).unwrap();
                }
            }
        }

        canvas.present();

        // FPS Limit
        std::thread::sleep(std::time::Duration::new(0, 1_000_000_000u32 / FPS_LIMIT /*fps_limit*/));
    }
}
