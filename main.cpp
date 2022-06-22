#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <time.h>

struct Particle{
    bool toMove;
    int type; // 0->stone, 1->wood, 2->water, 3->oil, 4->sand, 5->gunpowder, 6->fire, 7->lava, 8->steam, 9->smoke, 10->air
    SDL_Surface *image;
    SDL_Rect rect;
};

int main(int argv, char* args[]){
    //Init
    srand(time(NULL));
    
    if(SDL_Init(SDL_INIT_EVERYTHING)>0){
        std::cout<<"SDL Error: "<<std::endl<<SDL_GetError()<<std::endl;
    }

    SDL_Window *window=SDL_CreateWindow("Physics simulation",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,640,750,SDL_WINDOW_OPENGL);
    if(window==NULL){
        std::cout<<"SDL Error: "<<std::endl<<SDL_GetError()<<std::endl;
    }
    SDL_Surface *screen=SDL_GetWindowSurface(window);

    //Pre-loop stuff
    int particlePlacingId=0;
    const int particleColors[11][3]={
        {97,92,88}, //stone
        {168,126,41}, //wood
        {25,46,238}, //water
        {154,72,9}, //oil
        {250,230,46}, //sand
        {118,99,84}, //gunpowder
        {255,94,0}, //fire
        {255,34,0}, //lava
        {229,221,221}, //steam
        {47,43,43}, //smoke
        {0,0,0} //air
    };
    Particle grid[64][64];
    for(int i=0;i<64;i++){
        for(int j=0;j<64;j++){
            grid[i][j].type=10; // 0->stone, 1->wood, 2->water, 3->oil, 4->sand, 5->gunpowder, 6->fire, 7->lava, 8->steam, 9->smoke, 10->air
            grid[i][j].image=SDL_CreateRGBSurface(0,10,10,32,0,0,0,0);
            grid[i][j].rect=grid[i][j].image->clip_rect;
            grid[i][j].rect.x=0;
            grid[i][j].rect.y=0;
        }
    }

    SDL_Surface *uiImage=IMG_Load("ui.png");
    SDL_Rect uiRect=uiImage->clip_rect;
    uiRect.x=0;
    uiRect.y=640;

    SDL_Surface *uiSelectionImage=SDL_CreateRGBSurface(0,29,10,32,0,0,0,0);
    SDL_Rect uiSelectionRect=uiSelectionImage->clip_rect;
    SDL_FillRect(uiSelectionImage,NULL,SDL_MapRGB(screen->format,255,255,255));
    uiSelectionRect.y=715;

    bool isLeftMouseButtonDown=false;
    bool isRightMouseButtonDown=false;
    int mouseX, mouseY;

    Uint32 frameStartTick;
    SDL_Event event;
    bool running=true;

    //Main Loop
    while(running){
        frameStartTick=SDL_GetTicks();

        //Event loop
        while(SDL_PollEvent(&event)){
            if(event.type==SDL_QUIT){
                running=false;
                break;
            }
            else if(event.type==SDL_MOUSEWHEEL){
                if(event.wheel.y<0){
                    particlePlacingId++;
                    if(particlePlacingId==8) particlePlacingId=0;
                }else if(event.wheel.y>0){
                    particlePlacingId--;
                    if(particlePlacingId==-1) particlePlacingId=7;
                }
            }
            else if(event.type==SDL_MOUSEBUTTONDOWN){
                if(event.button.button==SDL_BUTTON_LEFT)
                isLeftMouseButtonDown=true;
                else if(event.button.button==SDL_BUTTON_RIGHT)
                isRightMouseButtonDown=true;
            }
            else if(event.type==SDL_MOUSEBUTTONUP){
                if(event.button.button==SDL_BUTTON_LEFT)
                isLeftMouseButtonDown=false;
                else if(event.button.button==SDL_BUTTON_RIGHT)
                isRightMouseButtonDown=false;
            }
            else if(event.type==SDL_KEYDOWN){
                if(event.key.keysym.sym==SDLK_r){
                    for(int i=0;i<64;i++){
                        for(int j=0;j<64;j++){
                            grid[i][j].type=10;
                        }
                    }
                }
                else if(event.key.keysym.sym==SDLK_q||event.key.keysym.sym==SDLK_ESCAPE){
                    running=false;
                    break;
                }
            }
        }

        //Mouse events
        SDL_PumpEvents();
        SDL_GetMouseState(&mouseX,&mouseY);
        if(isLeftMouseButtonDown&&mouseY<640&&mouseY>0&&mouseX<640&&mouseX>0){
            grid[mouseX/10][mouseY/10].type=particlePlacingId;
        }else if(isRightMouseButtonDown&&mouseY<640&&mouseY>0&&mouseX<640&&mouseX>0){
            grid[mouseX/10][mouseY/10].type=10;
        }
    
        //Every-frame stuff
        SDL_FillRect(screen,NULL,SDL_MapRGB(screen->format, 0,0,0));

        //Physics
        for(int i=0;i<64;i++){
            for(int j=0;j<64;j++){
                grid[i][j].toMove=true;
            }
        }   
        for(int i=0;i<64;i++){
            for(int j=0;j<64;j++){
                //Fire disappearing
                if(grid[i][j].type==6&&(rand() % 101==0||(grid[i][j-1].type!=10&&j>0))){
                    grid[i][j].type=9;
                    if(grid[i][j-1].type==10&&j>0){
                        grid[i][j-1].type=9;
                    }                    
                }

                //Fire spreading and water evaporation
                if(grid[i][j].type==6||grid[i][j].type==7){
                    if(rand()%101<4&&i<63&&(grid[i+1][j].type==1||grid[i+1][j].type==3||grid[i+1][j].type==5||grid[i+1][j].type==2)){
                        if(grid[i+1][j].type==2){
                            grid[i+1][j].type=8;
                            if(grid[i][j].type==7)
                            grid[i][j].type=0;
                        }
                        else
                        grid[i+1][j].type=6;
                    }
                    if(rand()%101<4&&i>0&&(grid[i-1][j].type==1||grid[i-1][j].type==3||grid[i-1][j].type==5||grid[i-1][j].type==2)){
                        if(grid[i-1][j].type==2){
                            grid[i-1][j].type=8;
                            if(grid[i][j].type==7)
                            grid[i][j].type=0;
                        }
                        else
                        grid[i-1][j].type=6;
                    }
                    if(rand()%101<4&&i<63&&j<63&&(grid[i+1][j+1].type==1||grid[i+1][j+1].type==3||grid[i+1][j+1].type==5||grid[i+1][j+1].type==2)){
                        if(grid[i+1][j+1].type==2){
                            grid[i+1][j+1].type=8;
                            if(grid[i][j].type==7)
                            grid[i][j].type=0;
                        }
                        else
                        grid[i+1][j+1].type=6;
                    }
                    if(rand()%101<4&&i>0&&j<63&&(grid[i-1][j+1].type==1||grid[i-1][j+1].type==3||grid[i-1][j+1].type==5||grid[i-1][j+1].type==2)){
                        if(grid[i-1][j+1].type==2){
                            grid[i-1][j+1].type=8;
                            if(grid[i][j].type==7)
                            grid[i][j].type=0;
                        }
                        else
                        grid[i-1][j+1].type=6;
                    }
                    if(rand()%101<4&&i>0&&j>0&&(grid[i-1][j-1].type==1||grid[i-1][j-1].type==3||grid[i-1][j-1].type==5||grid[i-1][j-1].type==2)){
                        if(grid[i-1][j-1].type==2){
                            grid[i-1][j-1].type=8;
                            if(grid[i][j].type==7)
                            grid[i][j].type=0;
                        }
                        else
                        grid[i-1][j-1].type=6;
                    }
                    if(rand()%101<4&&i<63&&j>0&&(grid[i+1][j-1].type==1||grid[i+1][j-1].type==3||grid[i+1][j-1].type==5||grid[i+1][j-1].type==2)){
                        if(grid[i+1][j-1].type==2){
                            grid[i+1][j-1].type=8;
                            if(grid[i][j].type==7)
                            grid[i][j].type=0;
                        }
                        else
                        grid[i+1][j-1].type=6;
                    }
                    if(rand()%101<4&&j<63&&(grid[i][j+1].type==1||grid[i][j+1].type==3||grid[i][j+1].type==5||grid[i][j+1].type==2)){
                        if(grid[i][j+1].type==2){
                            grid[i][j+1].type=8;
                        }
                        else
                        grid[i][j+1].type=6;
                    }
                    if(rand()%101<4&&j>0&&(grid[i][j-1].type==1||grid[i][j-1].type==3||grid[i][j-1].type==5||grid[i][j-1].type==2)){
                        if(grid[i][j-1].type==2){
                            grid[i][j-1].type=8;
                        }
                        else
                        grid[i][j-1].type=6;
                    }
                }

                //Falling and etc
                //Powders
                if(j<63&&(grid[i][j].type==4||grid[i][j].type==5)&&grid[i][j].toMove){
                    if(grid[i][j+1].type==10||grid[i][j+1].type==9||grid[i][j+1].type==8){
                        grid[i][j+1].type=grid[i][j].type;
                        grid[i][j+1].toMove=false;
                        grid[i][j].type=10;
                    }else if(i>0&&(grid[i-1][j+1].type==10||grid[i-1][j+1].type==9||grid[i-1][j+1].type==8)){
                        grid[i-1][j+1].type=grid[i][j].type;
                        grid[i-1][j+1].toMove=false;
                        grid[i][j].type=10;
                    }else if(i<63&&(grid[i+1][j+1].type==10||grid[i+1][j+1].type==9||grid[i+1][j+1].type==8)){
                        grid[i+1][j+1].type=grid[i][j].type;
                        grid[i+1][j+1].toMove=false;
                        grid[i][j].type=10;
                    }
                }//Liquids
                else if((grid[i][j].type==2||grid[i][j].type==3||grid[i][j].type==7)&&grid[i][j].toMove){
                    if(j<63&&(grid[i][j+1].type==10||grid[i][j+1].type==9||grid[i][j+1].type==8)){
                        grid[i][j+1].type=grid[i][j].type;
                        grid[i][j+1].toMove=false;
                        grid[i][j].type=10;
                    }else if(j<63&&i>0&&(grid[i-1][j+1].type==10||grid[i-1][j+1].type==9||grid[i-1][j+1].type==8)){
                        grid[i-1][j+1].type=grid[i][j].type;
                        grid[i-1][j+1].toMove=false;
                        grid[i][j].type=10;
                    }else if(j<63&&i<63&&(grid[i+1][j+1].type==10||grid[i+1][j+1].type==9||grid[i+1][j+1].type==8)){
                        grid[i+1][j+1].type=grid[i][j].type;
                        grid[i+1][j+1].toMove=false;
                        grid[i][j].type=10;
                    }else if(i>0&&(grid[i-1][j].type==10||grid[i-1][j].type==9||grid[i-1][j].type==8)){
                        grid[i-1][j].type=grid[i][j].type;
                        grid[i-1][j].toMove=false;
                        grid[i][j].type=10;
                    }else if(i<63&&(grid[i+1][j].type==10||grid[i+1][j].type==9||grid[i+1][j].type==8)){
                        grid[i+1][j].type=grid[i][j].type;
                        grid[i+1][j].toMove=false;
                        grid[i][j].type=10;
                    }
                }//Gases
                else if((grid[i][j].type==8||grid[i][j].type==9)&&grid[i][j].toMove){
                    if(j>0&&grid[i][j-1].type==10){
                        grid[i][j-1].type=grid[i][j].type;
                        grid[i][j-1].toMove=false;
                        grid[i][j].type=10;
                    }else if(j>0&&i>0&&grid[i-1][j-1].type==10){
                        grid[i-1][j-1].type=grid[i][j].type;
                        grid[i-1][j-1].toMove=false;
                        grid[i][j].type=10;
                    }else if(j>0&&i<63&&grid[i+1][j-1].type==10){
                        grid[i+1][j-1].type=grid[i][j].type;
                        grid[i+1][j-1].toMove=false;
                        grid[i][j].type=10;
                    }else if(i>0&&grid[i-1][j].type==10){
                        grid[i-1][j].type=grid[i][j].type;
                        grid[i-1][j].toMove=false;
                        grid[i][j].type=10;
                    }else if(i<63&&grid[i+1][j].type==10){
                        grid[i+1][j].type=grid[i][j].type;
                        grid[i+1][j].toMove=false;
                        grid[i][j].type=10;
                    }
                }
            }
        }
        //Particle rendering
        for(int i=0;i<64;i++){
            for(int j=0;j<64;j++){
                if(grid[i][j].type!=10){
                    SDL_FillRect(grid[i][j].image,NULL,SDL_MapRGB(screen->format,
                        particleColors[grid[i][j].type][0],particleColors[grid[i][j].type][1],particleColors[grid[i][j].type][2]
                    ));
                    grid[i][j].rect.x=i*10;
                    grid[i][j].rect.y=j*10;
                    SDL_BlitSurface(grid[i][j].image,NULL,screen,&grid[i][j].rect);
                }
            }
        }

        //UI
        SDL_BlitSurface(uiImage,NULL,screen,&uiRect);
        uiSelectionRect.x=185+(30*particlePlacingId);
        SDL_BlitSurface(uiSelectionImage,NULL,screen,&uiSelectionRect);

        if((1000/16)>SDL_GetTicks()-frameStartTick) SDL_Delay(1000/16-(SDL_GetTicks()-frameStartTick));
        SDL_UpdateWindowSurface(window);
    }

    SDL_FreeSurface(screen);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

