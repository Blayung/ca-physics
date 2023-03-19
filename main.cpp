#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <time.h>

struct Particle{
    bool toMove;
    int type; // 0->air, 1->stone, 2->wood, 3->water, 4->oil, 5->sand, 6->gunpowder, 7->fire, 8->lava, 9->steam, 10->smoke, 11->flammable gas
    SDL_Surface *image;
    SDL_Rect rect;
};

template<typename C, typename T>
bool isInThatList(C&& c, T e) { 
    return std::find(std::begin(c), std::end(c), e) != std::end(c);
};

std::string floatToString(float value){
    std::ostringstream temp;
    temp.precision(5);
    temp << value;
    return temp.str();
}

int main(int argv, char* args[]){
    //Init
    int gridSize;
    int particleSize;
    std::string temp;
    while(true) {
         std::cout<<"Enter the size of a single particle: ";
         std::cin>>temp;

         try{
             particleSize=std::stoi(temp);
         }catch(std::invalid_argument){
             std::cout<<"It has to be an integer!\n";
             continue;
         }
         if(particleSize<10){
             std::cout<<"Particle's size needs to be at least 10!\n";
         }else{
             break;
         }
     }
     while(true) {
         std::cout<<"Enter the size of the grid: ";
         std::cin>>temp;

         try{
             gridSize=std::stoi(temp);
         }catch(std::invalid_argument){
             std::cout<<"It has to be an integer!\n";
             continue;
         }
         if(gridSize<80){
             std::cout<<"Grid's size needs to be at least 80!\n";
         }else{
             temp.clear();
             break;
         }
     }

    srand(time(NULL));
    
    if(SDL_Init(SDL_INIT_EVERYTHING)<0)
        std::cout<<"SDL Error:"<<"\n"<<SDL_GetError()<<"\n";

    if(TTF_Init()<0)
        std::cout<<"SDL TTF Error:"<<"\n"<<TTF_GetError()<<"\n";

    SDL_Window *window=SDL_CreateWindow("Physics simulation",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,gridSize*particleSize,gridSize*particleSize+110,SDL_WINDOW_OPENGL);
    if(window==NULL){
        std::cout<<"SDL Error:"<<"\n"<<SDL_GetError()<<"\n";
    }
    SDL_Surface *screen=SDL_GetWindowSurface(window);

    // Pre-loop stuff
    //
    // 0->air, 1->stone, 2->wood, 3->water, 4->oil, 5->sand, 6->gunpowder, 7->fire, 8->lava, 9->steam, 10->smoke, 11->flammable gas
    const int AIR=0;
    const int STONE=1;
    const int WOOD=2;
    const int WATER=3;
    const int OIL=4;
    const int SAND=5;
    const int GUNPOWDER=6;
    const int FIRE=7;
    const int LAVA=8;
    const int STEAM=9;
    const int SMOKE=10;
    const int FLAMMABLE_GAS=11;

    const int SOLIDS[2]={1,2};
    const int POWDERS[2]={5,6};
    const int FLUIDS[3]={3,4,8};
    const int GASES[3]={9,10,11};

    const int STARTING_FIRE[2]={7,8};
    const int FLAMMABLE[4]={2,4,6,11};
    const int CAN_GO_THROUGH[4]={0,9,10,11};

    const int PARTICLE_COLORS[12][3]={
        {0,0,0}, //air
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
        {15,230,44} //flammable gas
    };

    Particle grid[gridSize][gridSize];
    for(int i=0;i<gridSize;i++){
        for(int j=0;j<gridSize;j++){
            grid[i][j].type=AIR;
            grid[i][j].image=SDL_CreateRGBSurface(0,particleSize,particleSize,32,0,0,0,0);
            grid[i][j].rect=grid[i][j].image->clip_rect;
            grid[i][j].rect.x=0;
            grid[i][j].rect.y=0;
        }
    }

    int particlePlacingId=1;
    int placingArea=1; //for later use
    int fps=32;

    SDL_Surface *uiLineImage=SDL_CreateRGBSurface(0,gridSize*particleSize,3,32,0,0,0,0);
    SDL_Rect uiLineRect=uiLineImage->clip_rect;
    SDL_FillRect(uiLineImage,NULL,SDL_MapRGB(screen->format,255,255,255));
    uiLineRect.x=0;
    uiLineRect.y=gridSize*particleSize;

    SDL_Surface *uiSelectionImage=SDL_CreateRGBSurface(0,30,10,32,0,0,0,0);
    SDL_Rect uiSelectionRect=uiSelectionImage->clip_rect;
    SDL_FillRect(uiSelectionImage,NULL,SDL_MapRGB(screen->format,255,255,255));
    uiSelectionRect.y=gridSize*particleSize+75;

    SDL_Surface *uiParticleImage;
    SDL_Rect uiParticleRect;

    TTF_Font *monospace=TTF_OpenFont("monospace.medium.ttf", 10);
    SDL_Surface *uiHintsSurface;
    SDL_Rect uiHintsRect;
    uiHintsRect.x=5;
    uiHintsRect.y=gridSize*particleSize+5;
    uiHintsRect.w=uiHintsSurface->w;
    uiHintsRect.h=uiHintsSurface->h;
    std::string uiHintsText;

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
                    if(particlePlacingId==sizeof(PARTICLE_COLORS)/sizeof(PARTICLE_COLORS[0])) particlePlacingId=1;
                }else if(event.wheel.y>0){
                    particlePlacingId--;
                    if(particlePlacingId==0) particlePlacingId=sizeof(PARTICLE_COLORS)/sizeof(PARTICLE_COLORS[0])-1;
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
                if(event.key.keysym.sym==SDLK_UP&&placingArea<10){
                    placingArea++;
                }
                if(event.key.keysym.sym==SDLK_DOWN&&placingArea>1){
                    placingArea--;
                }
                if(event.key.keysym.sym==SDLK_RIGHT&&fps<320){
                    fps+=2;
                }
                if(event.key.keysym.sym==SDLK_LEFT&&fps>8){
                    fps-=2;
                }

                if(event.key.keysym.sym==SDLK_r){
                    for(int i=0;i<gridSize;i++){
                        for(int j=0;j<gridSize;j++){
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
        if(isLeftMouseButtonDown&&mouseY<gridSize*particleSize&&mouseY>0&&mouseX<gridSize*particleSize&&mouseX>0){
            grid[mouseX/particleSize][mouseY/particleSize].type=particlePlacingId;
        }else if(isRightMouseButtonDown&&mouseY<gridSize*particleSize&&mouseY>0&&mouseX<gridSize*particleSize&&mouseX>0){
            grid[mouseX/particleSize][mouseY/particleSize].type=AIR;
        }
    
        //Every-frame stuff
        SDL_FillRect(screen,NULL,SDL_MapRGB(screen->format, 0,0,0));

        //Physics
        for(int i=0;i<gridSize;i++){
            for(int j=0;j<gridSize;j++){
                grid[i][j].toMove=true;
            }
        }   
        for(int i=0;i<gridSize;i++){
            for(int j=0;j<gridSize;j++){
                //Fire disappearing
                if(grid[i][j].type==FIRE&&rand() % 101==0){
                    grid[i][j].type=SMOKE;
                    if(grid[i][j-1].type==AIR&&j>0){
                        grid[i][j-1].type=SMOKE;
                    }                    
                }

                //Fire spreading, burning and water evaporation
                if(grid[i][j].type==FIRE||grid[i][j].type==LAVA){
                    if(rand()%150<2&&i<gridSize-1&&(isInThatList(FLAMMABLE,grid[i+1][j].type)||grid[i+1][j].type==WATER)){
                        if(grid[i+1][j].type==WATER){
                            grid[i+1][j].type=STEAM;
                            if(grid[i][j].type==LAVA)
                            grid[i][j].type=STONE;
                        }
                        else
                        grid[i+1][j].type=FIRE;
                    }
                    if(rand()%150<2&&i>0&&(isInThatList(FLAMMABLE,grid[i-1][j].type)||grid[i-1][j].type==WATER)){
                        if(grid[i-1][j].type==WATER){
                            grid[i-1][j].type=STEAM;
                            if(grid[i][j].type==LAVA)
                            grid[i][j].type=STONE;
                        }
                        else
                        grid[i-1][j].type=FIRE;
                    }
                    if(rand()%150<2&&i<gridSize-1&&j<gridSize-1&&(isInThatList(FLAMMABLE,grid[i+1][j+1].type)||grid[i+1][j+1].type==WATER)){
                        if(grid[i+1][j+1].type==WATER){
                            grid[i+1][j+1].type=STEAM;
                            if(grid[i][j].type==LAVA)
                            grid[i][j].type=STONE;
                        }
                        else
                        grid[i+1][j+1].type=FIRE;
                    }
                    if(rand()%150<2&&i>0&&j<gridSize-1&&(isInThatList(FLAMMABLE,grid[i-1][j+1].type)||grid[i-1][j+1].type==WATER)){
                        if(grid[i-1][j+1].type==WATER){
                            grid[i-1][j+1].type=STEAM;
                            if(grid[i][j].type==LAVA)
                            grid[i][j].type=STONE;
                        }
                        else
                        grid[i-1][j+1].type=FIRE;
                    }
                    if(rand()%150<2&&i>0&&j>0&&(isInThatList(FLAMMABLE,grid[i-1][j-1].type)||grid[i-1][j-1].type==WATER)){
                        if(grid[i-1][j-1].type==WATER){
                            grid[i-1][j-1].type=STEAM;
                            if(grid[i][j].type==LAVA)
                            grid[i][j].type=STONE;
                        }
                        else
                        grid[i-1][j-1].type=FIRE;
                    }
                    if(rand()%150<2&&i<gridSize-1&&j>0&&(isInThatList(FLAMMABLE,grid[i+1][j-1].type)||grid[i+1][j-1].type==WATER)){
                        if(grid[i+1][j-1].type==WATER){
                            grid[i+1][j-1].type=STEAM;
                            if(grid[i][j].type==LAVA)
                            grid[i][j].type=STONE;
                        }
                        else
                        grid[i+1][j-1].type=FIRE;
                    }
                    if(rand()%150<2&&j<gridSize-1&&(isInThatList(FLAMMABLE,grid[i][j+1].type)||grid[i][j+1].type==WATER)){
                        if(grid[i][j+1].type==WATER){
                            grid[i][j+1].type=STEAM;
                            if(grid[i][j].type==LAVA)
                            grid[i][j].type=STONE;
                        }
                        else
                        grid[i][j+1].type=FIRE;
                    }
                    if(rand()%150<2&&j>0&&(isInThatList(FLAMMABLE,grid[i][j-1].type)||grid[i][j-1].type==WATER)){
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
                    if(j<gridSize-1&&isInThatList(CAN_GO_THROUGH,grid[i][j+1].type)){
                        grid[i][j+1].type=grid[i][j].type;
                        grid[i][j+1].toMove=false;
                        grid[i][j].type=AIR;
                    }
                    else if(j<gridSize-1&&i>0&&isInThatList(CAN_GO_THROUGH,grid[i-1][j+1].type)){
                        grid[i-1][j+1].type=grid[i][j].type;
                        grid[i-1][j+1].toMove=false;
                        grid[i][j].type=AIR;
                    }
                    else if(j<gridSize-1&&i<gridSize-1&&isInThatList(CAN_GO_THROUGH,grid[i+1][j+1].type)){
                        grid[i+1][j+1].type=grid[i][j].type;
                        grid[i+1][j+1].toMove=false;
                        grid[i][j].type=AIR;
                    }
                }
                //Fluids
                else if(isInThatList(FLUIDS,grid[i][j].type)&&grid[i][j].toMove){
                    if(j<gridSize-1&&isInThatList(CAN_GO_THROUGH,grid[i][j+1].type)){
                        grid[i][j+1].type=grid[i][j].type;
                        grid[i][j+1].toMove=false;
                        grid[i][j].type=AIR;
                    }
                    else if(j<gridSize-1&&i>0&&isInThatList(CAN_GO_THROUGH,grid[i-1][j+1].type)){
                        grid[i-1][j+1].type=grid[i][j].type;
                        grid[i-1][j+1].toMove=false;
                        grid[i][j].type=AIR;
                    }
                    else if(j<gridSize-1&&i<gridSize-1&&isInThatList(CAN_GO_THROUGH,grid[i+1][j+1].type)){
                        grid[i+1][j+1].type=grid[i][j].type;
                        grid[i+1][j+1].toMove=false;
                        grid[i][j].type=AIR;
                    }
                    else if(isInThatList(FLUIDS,grid[i][j+1].type)){
                        if(i>0&&isInThatList(CAN_GO_THROUGH,grid[i-1][j].type)){
                            grid[i-1][j].type=grid[i][j].type;
                            grid[i-1][j].toMove=false;
                            grid[i][j].type=AIR;
                        }
                        else if(i<gridSize-1&&isInThatList(CAN_GO_THROUGH,grid[i+1][j].type)){
                            grid[i+1][j].type=grid[i][j].type;
                            grid[i+1][j].toMove=false;
                            grid[i][j].type=AIR;
                        }
                    }
                }
                //Gases
                else if(isInThatList(GASES,grid[i][j].type)&&grid[i][j].toMove){
                    if(j>0&&grid[i][j-1].type==AIR){
                        grid[i][j-1].type=grid[i][j].type;
                        grid[i][j-1].toMove=false;
                        grid[i][j].type=AIR;
                    }else if(j>0&&i>0&&grid[i-1][j-1].type==AIR){
                        grid[i-1][j-1].type=grid[i][j].type;
                        grid[i-1][j-1].toMove=false;
                        grid[i][j].type=AIR;
                    }else if(j>0&&i<gridSize-1&&grid[i+1][j-1].type==AIR){
                        grid[i+1][j-1].type=grid[i][j].type;
                        grid[i+1][j-1].toMove=false;
                        grid[i][j].type=AIR;
                    }else if(isInThatList(GASES,grid[i][j-1].type)){
                        if(i>0&&grid[i-1][j].type==AIR){
                            grid[i-1][j].type=grid[i][j].type;
                            grid[i-1][j].toMove=false;
                            grid[i][j].type=AIR;
                        }else if(i<gridSize-1&&grid[i+1][j].type==AIR){
                            grid[i+1][j].type=grid[i][j].type;
                            grid[i+1][j].toMove=false;
                            grid[i][j].type=AIR;
                        }
                    }
                }
            }
        }

        //Particle rendering
        for(int i=0;i<gridSize;i++){
            for(int j=0;j<gridSize;j++){
                if(grid[i][j].type!=AIR){
                    SDL_FillRect(grid[i][j].image,NULL,SDL_MapRGB(screen->format,
                        PARTICLE_COLORS[grid[i][j].type][0],PARTICLE_COLORS[grid[i][j].type][1],PARTICLE_COLORS[grid[i][j].type][2]
                    ));
                    grid[i][j].rect.x=i*particleSize;
                    grid[i][j].rect.y=j*particleSize;
                    SDL_BlitSurface(grid[i][j].image,NULL,screen,&grid[i][j].rect);
                }
            }
        }

        //UI
        uiSelectionRect.x=((gridSize*particleSize-31*(sizeof(PARTICLE_COLORS)/sizeof(PARTICLE_COLORS[0])-1))/2)+(31*(particlePlacingId-1));
        SDL_BlitSurface(uiSelectionImage,NULL,screen,&uiSelectionRect);
        uiHintsText="Lmb/Rmb - Place/Delete\nScroll - Change selected particle\nR - Reset the grid\nQ/Escape - Quit\nLeft/Right arrow - Change speed\nSpeed: "+floatToString((float)fps/32)+"x";
        uiHintsSurface=TTF_RenderText_Blended_Wrapped(monospace, uiHintsText.c_str(), {255,255,255}, 0);
        SDL_BlitSurface(uiHintsSurface,NULL,screen,&uiHintsRect);
        SDL_BlitSurface(uiLineImage,NULL,screen,&uiLineRect);
        for(int i=1;i<sizeof(PARTICLE_COLORS)/sizeof(PARTICLE_COLORS[0]);i++){
            uiParticleImage=SDL_CreateRGBSurface(0,30,30,32,0,0,0,0);
            uiParticleRect=uiParticleImage->clip_rect;
            SDL_FillRect(uiParticleImage,NULL,SDL_MapRGB(screen->format,PARTICLE_COLORS[i][0],PARTICLE_COLORS[i][1],PARTICLE_COLORS[i][2]));
            uiParticleRect.x=((gridSize*particleSize-31*(sizeof(PARTICLE_COLORS)/sizeof(PARTICLE_COLORS[0])-1))/2)+(31*(i-1));
            uiParticleRect.y=gridSize*particleSize+40;
            SDL_BlitSurface(uiParticleImage,NULL,screen,&uiParticleRect);
        }

        if((1000/fps)>SDL_GetTicks()-frameStartTick) SDL_Delay(1000/fps-(SDL_GetTicks()-frameStartTick));
        SDL_UpdateWindowSurface(window);
    }

    return 0;
}
