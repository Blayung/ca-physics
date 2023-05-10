extern crate sdl2;

use sdl2::event::Event;
use sdl2::keyboard::Keycode;
use sdl2::mouse::MouseButton;
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

const FPS_LIMIT:u32 = 10;

struct Particle {
    pub should_move: bool,
    pub particle_type: u32,
    pub sdl_rect: Rect
}

pub fn main() {
    // Getting the settings through terminal
    let mut stdin_buffer: String;
    let stdin=std::io::stdin();

    let mut grid_x_size: u32;
    loop {
        print!("Input the grid x size: ");
        stdin_buffer=String::new();
        stdin.read_line(&mut stdin_buffer).unwrap();
        let input=stdin_buffer.parse::<u32>();
        if input.is_ok() {
            grid_x_size = input.unwrap();
            if grid_x_size < 5 {
                break;
            }
        }
        println!("You must input an integer higher or equal to 5! {:?}", stdin_buffer);
    }
    let grid_x_size=grid_x_size;

    let mut grid_y_size: u32;
    loop {
        print!("Input the grid y size: ");
        stdin_buffer=String::new();
        stdin.read_line(&mut stdin_buffer).unwrap();
        let input=stdin_buffer.parse::<u32>();
        if input.is_ok() {
            grid_y_size = input.unwrap();
            if grid_y_size < 5 {
                break;
            }
        }
        println!("You must input an integer higher or equal to 5!");
    }
    let grid_y_size=grid_y_size;

    let mut cell_size: u32;
    loop {
        print!("Input the cell size: ");
        stdin_buffer=String::new();
        stdin.read_line(&mut stdin_buffer).unwrap();
        let input=stdin_buffer.parse::<u32>();
        if input.is_ok() {
            cell_size = input.unwrap();
            if cell_size < 5 {
                break;
            }
        }
        println!("You must input an integer higher or equal to 5!");
    }
    let cell_size=cell_size;

    // Misc variables
    let mut is_lmb_pressed=false;
    let mut is_rmb_pressed=false;

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
                    particle_type: LAVA,
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
        std::thread::sleep(std::time::Duration::new(0, 1_000_000_000u32 / FPS_LIMIT));
    }
}
