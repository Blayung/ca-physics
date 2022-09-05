#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <algorithm>
#include <time.h>

struct Particle{
    bool toMove;
    int type; // 0->stone, 1->wood, 2->water, 3->oil, 4->sand, 5->gunpowder, 6->fire, 7->lava, 8->steam, 9->smoke, 10->air
    SDL_Surface *image;
    SDL_Rect rect;
};

template<typename C, typename T>
bool isInThatList(C&& c, T e) { 
    return std::find(std::begin(c), std::end(c), e) != std::end(c);
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

    // Pre-loop stuff
    //
    // 0->stone, 1->wood, 2->water, 3->oil, 4->sand, 5->gunpowder, 6->fire, 7->lava, 8->steam, 9->smoke, 10->air
    const int STONE=0;
    const int WOOD=1;
    const int WATER=2;
    const int OIL=3;
    const int SAND=4;
    const int GUNPOWDER=5;
    const int FIRE=6;
    const int LAVA=7;
    const int STEAM=8;
    const int SMOKE=9;
    const int AIR=10;

    const int SOLIDS[2]={0,1};
    const int POWDERS[2]={4,5};
    const int FLUIDS[3]={2,3,7};
    const int GASES[2]={9,8};

    const int STARTING_FIRE[2]={6,7};
    const int FLAMMABLE[3]={1,3,5};
    const int CAN_GO_THROUGH[3]={10,9,8};

    const int PARTICLE_COLORS[11][3]={
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
            grid[i][j].type=AIR;
            grid[i][j].image=SDL_CreateRGBSurface(0,10,10,32,0,0,0,0);
            grid[i][j].rect=grid[i][j].image->clip_rect;
            grid[i][j].rect.x=0;
            grid[i][j].rect.y=0;
        }
    }

    int particlePlacingId=0;

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
                            grid[i][j].type=AIR;
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
                if(grid[i][j].type==FIRE&&(rand() % 101==0||(grid[i][j-1].type!=AIR&&j>0))){
                    grid[i][j].type=SMOKE;
                    if(grid[i][j-1].type==AIR&&j>0){
                        grid[i][j-1].type=SMOKE;
                    }                    
                }

                //Fire spreading, burning and water evaporation
                if(grid[i][j].type==6||grid[i][j].type==7){
                    if(rand()%101<4&&i<63&&(isInThatList(FLAMMABLE,grid[i+1][j].type)||grid[i+1][j].type==WATER)){
                        if(grid[i+1][j].type==WATER){
                            grid[i+1][j].type=STEAM;
                            if(grid[i][j].type==LAVA)
                            grid[i][j].type=STONE;
                        }
                        else
                        grid[i+1][j].type=FIRE;
                    }
                    if(rand()%101<4&&i>0&&(isInThatList(FLAMMABLE,grid[i-1][j].type)||grid[i-1][j].type==WATER)){
                        if(grid[i-1][j].type==WATER){
                            grid[i-1][j].type=STEAM;
                            if(grid[i][j].type==LAVA)
                            grid[i][j].type=STONE;
                        }
                        else
                        grid[i-1][j].type=FIRE;
                    }
                    if(rand()%101<4&&i<63&&j<63&&(isInThatList(FLAMMABLE,grid[i+1][j+1].type)||grid[i+1][j+1].type==WATER)){
                        if(grid[i+1][j+1].type==WATER){
                            grid[i+1][j+1].type=STEAM;
                            if(grid[i][j].type==LAVA)
                            grid[i][j].type=STONE;
                        }
                        else
                        grid[i+1][j+1].type=FIRE;
                    }
                    if(rand()%101<4&&i>0&&j<63&&(isInThatList(FLAMMABLE,grid[i-1][j+1].type)||grid[i-1][j+1].type==WATER)){
                        if(grid[i-1][j+1].type==WATER){
                            grid[i-1][j+1].type=STEAM;
                            if(grid[i][j].type==LAVA)
                            grid[i][j].type=STONE;
                        }
                        else
                        grid[i-1][j+1].type=FIRE;
                    }
                    if(rand()%101<4&&i>0&&j>0&&(isInThatList(FLAMMABLE,grid[i-1][j-1].type)||grid[i-1][j-1].type==WATER)){
                        if(grid[i-1][j-1].type==WATER){
                            grid[i-1][j-1].type=STEAM;
                            if(grid[i][j].type==LAVA)
                            grid[i][j].type=STONE;
                        }
                        else
                        grid[i-1][j-1].type=FIRE;
                    }
                    if(rand()%101<4&&i<63&&j>0&&(isInThatList(FLAMMABLE,grid[i+1][j-1].type)||grid[i+1][j-1].type==WATER)){
                        if(grid[i+1][j-1].type==WATER){
                            grid[i+1][j-1].type=STEAM;
                            if(grid[i][j].type==LAVA)
                            grid[i][j].type=STONE;
                        }
                        else
                        grid[i+1][j-1].type=FIRE;
                    }
                    if(rand()%101<4&&j<63&&(isInThatList(FLAMMABLE,grid[i][j+1].type)||grid[i][j+1].type==WATER)){
                        if(grid[i][j+1].type==WATER){
                            grid[i][j+1].type=STEAM;
                            if(grid[i][j].type==LAVA)
                            grid[i][j].type=STONE;
                        }
                        else
                        grid[i][j+1].type=FIRE;
                    }
                    if(rand()%101<4&&j>0&&(isInThatList(FLAMMABLE,grid[i][j-1].type)||grid[i][j-1].type==WATER)){
                        if(grid[i][j-1].type==WATER){
                            grid[i][j-1].type=STEAM;
                            if(grid[i][j].type==LAVA)
                            grid[i][j].type=STONE;
                        }
                        else
                        grid[i][j-1].type=FIRE;
                    }
                }

                //Falling and etc
                //Powders
                if(isInThatList(POWDERS,grid[i][j].type)&&grid[i][j].toMove){
                    if(j<63&&isInThatList(CAN_GO_THROUGH,grid[i][j+1].type)){
                        grid[i][j+1].type=grid[i][j].type;
                        grid[i][j+1].toMove=false;
                        grid[i][j].type=10;
                    }
                    else if(j<63&&i>0&&isInThatList(CAN_GO_THROUGH,grid[i-1][j+1].type)){
                        grid[i-1][j+1].type=grid[i][j].type;
                        grid[i-1][j+1].toMove=false;
                        grid[i][j].type=10;
                    }
                    else if(j<63&&i<63&&isInThatList(CAN_GO_THROUGH,grid[i+1][j+1].type)){
                        grid[i+1][j+1].type=grid[i][j].type;
                        grid[i+1][j+1].toMove=false;
                        grid[i][j].type=10;
                    }
                }
                //Fluids
                else if(isInThatList(FLUIDS,grid[i][j].type)&&grid[i][j].toMove){
                    if(j<63&&isInThatList(CAN_GO_THROUGH,grid[i][j+1].type)){
                        grid[i][j+1].type=grid[i][j].type;
                        grid[i][j+1].toMove=false;
                        grid[i][j].type=AIR;
                    }
                    else if(j<63&&i>0&&isInThatList(CAN_GO_THROUGH,grid[i-1][j+1].type)){
                        grid[i-1][j+1].type=grid[i][j].type;
                        grid[i-1][j+1].toMove=false;
                        grid[i][j].type=AIR;
                    }
                    else if(j<63&&i<63&&isInThatList(CAN_GO_THROUGH,grid[i+1][j+1].type)){
                        grid[i+1][j+1].type=grid[i][j].type;
                        grid[i+1][j+1].toMove=false;
                        grid[i][j].type=AIR;
                    }
                    else if(i>0&&isInThatList(CAN_GO_THROUGH,grid[i-1][j].type)){
                        grid[i-1][j].type=grid[i][j].type;
                        grid[i-1][j].toMove=false;
                        grid[i][j].type=AIR;
                    }
                    else if(i<63&&isInThatList(CAN_GO_THROUGH,grid[i+1][j].type)){
                        grid[i+1][j].type=grid[i][j].type;
                        grid[i+1][j].toMove=false;
                        grid[i][j].type=AIR;
                    }
                }
                //Gases
                else if(isInThatList(GASES,grid[i][j].type)&&grid[i][j].toMove){
                    if(j>0&&grid[i][j-1].type==AIR){
                        grid[i][j-1].type=grid[i][j].type;
                        grid[i][j-1].toMove=false;
                        grid[i][j].type=AIR;
                    }else if(j>0&&i>0&&grid[i-1][j-1].type==10){
                        grid[i-1][j-1].type=grid[i][j].type;
                        grid[i-1][j-1].toMove=false;
                        grid[i][j].type=AIR;
                    }else if(j>0&&i<63&&grid[i+1][j-1].type==AIR){
                        grid[i+1][j-1].type=grid[i][j].type;
                        grid[i+1][j-1].toMove=false;
                        grid[i][j].type=AIR;
                    }else if(i>0&&grid[i-1][j].type==AIR){
                        grid[i-1][j].type=grid[i][j].type;
                        grid[i-1][j].toMove=false;
                        grid[i][j].type=AIR;
                    }else if(i<63&&grid[i+1][j].type==AIR){
                        grid[i+1][j].type=grid[i][j].type;
                        grid[i+1][j].toMove=false;
                        grid[i][j].type=AIR;
                    }
                }
            }
        }

        //Particle rendering
        for(int i=0;i<64;i++){
            for(int j=0;j<64;j++){
                if(grid[i][j].type!=AIR){
                    SDL_FillRect(grid[i][j].image,NULL,SDL_MapRGB(screen->format,
                        PARTICLE_COLORS[grid[i][j].type][0],PARTICLE_COLORS[grid[i][j].type][1],PARTICLE_COLORS[grid[i][j].type][2]
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