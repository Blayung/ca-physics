// Materials
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

const SOLIDS:[u32;2] = [STONE,WOOD];
const POWDERS:[u32;2] = [SAND,COAL];
const FLUIDS:[u32;3] = [WATER,OIL,LAVA];
const GASES:[u32;3] = [SMOKE,STEAM,FLAMMABLE_GAS];
const STARTING_FIRE:[u32;2] = [FIRE,LAVA];
const FLAMMABLE:[u32;4] = [WOOD,COAL,OIL,FLAMMABLE_GAS];
const CAN_GO_THROUGH:[u32;5] = [AIR,FIRE,SMOKE,STEAM,FLAMMABLE_GAS];

const MATERIAL_DENSITY: [i32;12] = [
    0, // AIR
    5, // STONE
    5, // WOOD
    2, // SAND
    4, // COAL
    1, // WATER
    2, // OIL
    3, // LAVA
    5, // FIRE
    2, // SMOKE
    1, // STEAM
    3 // FLAMMABLE_GAS
];

const MATERIAL_COLORS: [sdl2::pixels::Color;12] = [
    sdl2::pixels::Color::RGB(0, 0, 0), // AIR
    sdl2::pixels::Color::RGB(90, 90, 90), // STONE
    sdl2::pixels::Color::RGB(150, 130, 40), // WOOD
    sdl2::pixels::Color::RGB(250, 230, 40), // SAND
    sdl2::pixels::Color::RGB(40, 40, 40), // COAL
    sdl2::pixels::Color::RGB(20, 40, 230), // WATER
    sdl2::pixels::Color::RGB(120, 70, 10), // OIL
    sdl2::pixels::Color::RGB(255, 30, 0), // LAVA
    sdl2::pixels::Color::RGB(255, 100, 0), // FIRE
    sdl2::pixels::Color::RGB(50, 50, 50), // SMOKE
    sdl2::pixels::Color::RGB(220, 220, 220), //STEAM
    sdl2::pixels::Color::RGB(20, 230, 140) // FLAMMABLE_GAS
];

#[derive(Copy, Clone)]
struct Particle {
    pub should_move: bool,
    pub particle_type: u32,
    pub sdl_rect: sdl2::rect::Rect
}

/*
THE FOLLOWING FUNCTION IS A RUST PORT OF MATTBATWINGS' PYTHON CODE FROM HIS REDSTONE VIDEO!
https://youtu.be/vfPGuUDuwmo
https://imgur.com/a/2uT7LaV
*/
fn calculate_line(point_a: (i32,i32), point_b: (i32,i32)) -> std::vec::Vec<(i32,i32)> {
    let mut output: std::vec::Vec<(i32,i32)>=std::vec::Vec::new();

    let bx_minus_ax = point_b.0 - point_a.0;
    let by_minus_ay = point_b.1 - point_a.1;
    let increment_x = bx_minus_ax.signum();
    let increment_y = by_minus_ay.signum();
    let difference_x = bx_minus_ax.abs();
    let difference_y = by_minus_ay.abs();

    // I'm not sure what these three variables represent, and I don't wanna waste my time on it, so I won't refactor them further.
    let mut cmpt = difference_x.max(difference_y);
    let inc_d = -2 * (difference_x - difference_y).abs();
    let inc_s = 2 * difference_x.min(difference_y);

    let mut error = inc_d + cmpt;
    let mut x = point_a.0;
    let mut y = point_a.1;

    while cmpt >= 0 {
        output.push((x,y));
        cmpt -= 1;

        if error>=0 || difference_x > difference_y {
            x+=increment_x;
        }
        if error>=0 || difference_x < difference_y {
            y+=increment_y;
        }

        if error>=0 {
            error+=inc_d;
        } else {
            error+=inc_s;
        }
    }

    return output;
}

fn swap_grid_values(vector:&mut std::vec::Vec<std::vec::Vec<Particle>>, element_1_x:usize, element_1_y:usize, element_2_x:usize, element_2_y:usize){
    let buffer = vector[element_1_x][element_1_y];
    vector[element_1_x][element_1_y] = vector[element_2_x][element_2_y];
    vector[element_2_x][element_2_y] = buffer;
}

fn main() {
    let grid_x_size:u32=64;
    let grid_y_size:u32=64;
    let cell_size:u32=10;
    let fps_limit:u32=64;

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
                    sdl_rect: sdl2::rect::Rect::new((x * cell_size) as i32, (y * cell_size) as i32, cell_size, cell_size)
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
    let mut frame_start_time: std::time::Instant;
    let mut mouse_state: sdl2::mouse::MouseState;
    let mut should_read_last_mouse_xy = false;
    let mut last_mouse_xy: (i32,i32) = (0,0);
    let mut current_particle = 1;

    // SDL Vars
    let sdl_context = sdl2::init().unwrap();
    let video_subsystem = sdl_context.video().unwrap();
    let window = video_subsystem.window("Physics simulation", grid_y_size * cell_size, grid_x_size * cell_size).position_centered().opengl().build().unwrap();
    let mut canvas = window.into_canvas().build().unwrap();
    let mut event_pump = sdl_context.event_pump().unwrap();

    // Main loop
    'main_loop: loop {
        frame_start_time = std::time::Instant::now();

        // Events
        for event in event_pump.poll_iter() {
            match event {
                sdl2::event::Event::Quit {..} | sdl2::event::Event::KeyDown { keycode: Some(sdl2::keyboard::Keycode::Escape), .. } | sdl2::event::Event::KeyDown { keycode: Some(sdl2::keyboard::Keycode::Q), .. } => break 'main_loop,

                sdl2::event::Event::KeyDown { keycode: Some(sdl2::keyboard::Keycode::R), .. } => {
                    for x in &mut grid {
                        for mut y in x {
                            y.particle_type=AIR;
                        }
                    }
                },

                sdl2::event::Event::KeyDown { keycode: Some(sdl2::keyboard::Keycode::Backquote), .. } => current_particle=1,
                sdl2::event::Event::KeyDown { keycode: Some(sdl2::keyboard::Keycode::Num1), .. } => current_particle=2,
                sdl2::event::Event::KeyDown { keycode: Some(sdl2::keyboard::Keycode::Num2), .. } => current_particle=3,
                sdl2::event::Event::KeyDown { keycode: Some(sdl2::keyboard::Keycode::Num3), .. } => current_particle=4,
                sdl2::event::Event::KeyDown { keycode: Some(sdl2::keyboard::Keycode::Num4), .. } => current_particle=5,
                sdl2::event::Event::KeyDown { keycode: Some(sdl2::keyboard::Keycode::Num5), .. } => current_particle=6,
                sdl2::event::Event::KeyDown { keycode: Some(sdl2::keyboard::Keycode::Num6), .. } => current_particle=7,
                sdl2::event::Event::KeyDown { keycode: Some(sdl2::keyboard::Keycode::Num7), .. } => current_particle=8,
                sdl2::event::Event::KeyDown { keycode: Some(sdl2::keyboard::Keycode::Num8), .. } => current_particle=9,
                sdl2::event::Event::KeyDown { keycode: Some(sdl2::keyboard::Keycode::Num9), .. } => current_particle=10,
                sdl2::event::Event::KeyDown { keycode: Some(sdl2::keyboard::Keycode::Num0), .. } => current_particle=11,

                sdl2::event::Event::MouseWheel { direction, y, .. } => {
                    if direction == sdl2::mouse::MouseWheelDirection::Flipped {
                        if y<0 {
                            current_particle+=1;
                        } else {
                            current_particle-=1;
                        }

                        if current_particle<1 {
                            current_particle=MATERIAL_COLORS.len();
                        }else if current_particle>MATERIAL_COLORS.len(){
                            current_particle=1;
                        }
                    } else {
                        if y>0 {
                            current_particle+=1;
                        } else {
                            current_particle-=1;
                        }

                        if current_particle<1 {
                            current_particle=MATERIAL_COLORS.len()-1;
                        }else if current_particle>MATERIAL_COLORS.len()-1{
                            current_particle=1;
                        }
                    }
                },
                _ => {}
            }
        }

        mouse_state=event_pump.mouse_state();
        if mouse_state.left()
        {
            let xy=(mouse_state.x()/cell_size as i32,mouse_state.y()/cell_size as i32);
            let line: std::vec::Vec<(i32,i32)>;
            if should_read_last_mouse_xy {
                line=calculate_line((last_mouse_xy.0,last_mouse_xy.1),xy);
            } else {
                line=calculate_line(xy,xy);
            }

            for i in line {
                if i.0>=0 && i.0<=grid_x_size as i32 && i.1>=0 && i.1<=grid_y_size as i32 {
                    grid[i.0 as usize][i.1 as usize].particle_type=current_particle as u32;
                }
            }

            last_mouse_xy=(mouse_state.x()/cell_size as i32,mouse_state.y()/cell_size as i32);
            should_read_last_mouse_xy=true;
        }
        else if mouse_state.right()
        {
            let xy=(mouse_state.x()/cell_size as i32,mouse_state.y()/cell_size as i32);
            let line: std::vec::Vec<(i32,i32)>;
            if should_read_last_mouse_xy {
                line=calculate_line((last_mouse_xy.0,last_mouse_xy.1),xy);
            } else {
                line=calculate_line(xy,xy);
            }

            for i in line {
                if i.0>=0 && i.0<=grid_x_size as i32 && i.1>=0 && i.1<=grid_y_size as i32 {
                    grid[i.0 as usize][i.1 as usize].particle_type=AIR;
                }
            }

            last_mouse_xy=(mouse_state.x()/cell_size as i32,mouse_state.y()/cell_size as i32);
            should_read_last_mouse_xy=true;
        } else {
            should_read_last_mouse_xy=false;
        }
        
        // Physics
        for x in &mut grid {
            for mut y in x {
                y.should_move=true;
            }
        }

        let mut x:usize=0;
        loop {
            let mut y:usize=0;
            loop {
                //Powders
                if POWDERS.contains(&grid[x][y].particle_type) && grid[x][y].should_move {
                    if y<grid_y_size as usize-1 && CAN_GO_THROUGH.contains(&grid[x][y+1].particle_type) {
                        swap_grid_values(&mut grid,64,64,64,0);
                        grid[x][y+1].particle_type = grid[x][y].particle_type;
                        grid[x][y+1].should_move = false;
                        grid[x][y].particle_type = AIR;
                    }
                    else if y<grid_y_size as usize-1 && x>0 && CAN_GO_THROUGH.contains(&grid[x-1][y+1].particle_type) {
                        grid[x-1][y+1].particle_type = grid[x][y].particle_type;
                        grid[x-1][y+1].should_move = false;
                        grid[x][y].particle_type = AIR;
                    }
                    else if y<grid_y_size as usize-1 && x<grid_x_size as usize-1 && CAN_GO_THROUGH.contains(&grid[x+1][y+1].particle_type) {
                        grid[x+1][y+1].particle_type = grid[x][y].particle_type;
                        grid[x+1][y+1].should_move = false;
                        grid[x][y].particle_type = AIR;
                    }
                }
                //Fluids
                else if FLUIDS.contains(&grid[x][y].particle_type) && grid[x][y].should_move {
                    if y<grid_y_size as usize-1 && CAN_GO_THROUGH.contains(&grid[x][y+1].particle_type) {
                        grid[x][y+1].particle_type = grid[x][y].particle_type;
                        grid[x][y+1].should_move = false;
                        grid[x][y].particle_type = AIR;
                    }
                    else if y<grid_y_size as usize-1 && x>0 && CAN_GO_THROUGH.contains(&grid[x-1][y+1].particle_type) {
                        grid[x-1][y+1].particle_type = grid[x][y].particle_type;
                        grid[x-1][y+1].should_move = false;
                        grid[x][y].particle_type = AIR;
                    }
                    else if y<grid_y_size as usize-1 && x<grid_x_size as usize-1 && CAN_GO_THROUGH.contains(&grid[x+1][y+1].particle_type) {
                        grid[x+1][y+1].particle_type = grid[x][y].particle_type;
                        grid[x+1][y+1].should_move = false;
                        grid[x][y].particle_type = AIR;
                    }
                    else if FLUIDS.contains(&grid[x][y+1].particle_type) {
                        if x>0 && CAN_GO_THROUGH.contains(&grid[x-1][y].particle_type) {
                            grid[x-1][y].particle_type = grid[x][y].particle_type;
                            grid[x-1][y].should_move = false;
                            grid[x][y].particle_type = AIR;
                        }
                        else if x<grid_x_size as usize-1 && CAN_GO_THROUGH.contains(&grid[x+1][y].particle_type) {
                            grid[x+1][y].particle_type = grid[x][y].particle_type;
                            grid[x+1][y].should_move = false;
                            grid[x][y].particle_type = AIR;
                        }
                    }
                }
                //Gases
                else if GASES.contains(&grid[x][y].particle_type) && grid[x][y].should_move {
                    if y>0 && grid[x][y-1].particle_type == AIR {
                        grid[x][y-1].particle_type = grid[x][y].particle_type;
                        grid[x][y-1].should_move = false;
                        grid[x][y].particle_type = AIR;
                    }
                    else if y>0 && x>0 && grid[x-1][y-1].particle_type == AIR {
                        grid[x-1][y-1].particle_type = grid[x][y].particle_type;
                        grid[x-1][y-1].should_move = false;
                        grid[x][y].particle_type = AIR;
                    }
                    else if y>0 && x<grid_x_size as usize-1 && grid[x+1][y-1].particle_type == AIR {
                        grid[x+1][y-1].particle_type = grid[x][y].particle_type;
                        grid[x+1][y-1].should_move = false;
                        grid[x][y].particle_type = AIR;
                    }
                    else if y>0 {
                        if GASES.contains(&grid[x][y-1].particle_type) {
                            if x>0 && grid[x-1][y].particle_type == AIR {
                                grid[x-1][y].particle_type = grid[x][y].particle_type;
                                grid[x-1][y].should_move = false;
                                grid[x][y].particle_type = AIR;
                            }
                            else if x<grid_x_size as usize-1 && grid[x+1][y].particle_type == AIR {
                                grid[x+1][y].particle_type = grid[x][y].particle_type;
                                grid[x+1][y].should_move = false;
                                grid[x][y].particle_type = AIR;
                            }
                        }
                    }
                }

                y+=1;
                if y>grid_y_size as usize {
                    break;
                }
            }
            x+=1;
            if x>grid_x_size as usize {
                break;
            }
        }

        // Drawing to the screen
        canvas.set_draw_color(sdl2::pixels::Color::RGB(0, 0, 0));
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
        std::thread::sleep(std::time::Duration::new(0, 1_000_000_000u32 / fps_limit).checked_sub(frame_start_time.elapsed()).unwrap_or(std::time::Duration::new(0,0)));
    }
}
