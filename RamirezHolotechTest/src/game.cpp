#include "game.h"
#include "object.h"

#define ENEMY_RATE      0.15
#define ENEMY_SPEED     10
#define PLAYER_SPEED    5

Game::Game(){}
Game::~Game(){}

// Initialize gaming window
void Game::init(const char *title, int width, int height, bool fullscreen){
    int flags = SDL_WINDOW_RESIZABLE;
    if(fullscreen){
        flags |= SDL_WINDOW_FULLSCREEN;
    }

    // set title, width, and height for window
    this->title = title;
    this->width = width;
    this->height = height;

    // Initialize window, renderer, colors
    if(SDL_Init(SDL_INIT_EVERYTHING) == 0){
        window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
        if(window){
            std::cout << "Window created" << std::endl;
        }
        renderer = SDL_CreateRenderer(window, -1, 0);
        if(renderer){
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        }
        if(TTF_Init() >= 0){
            std::cout << "TTF initialized" << std::endl;   
        }
        else{
            std::cout << "Error initializing TTF" << std::endl;
        }
        isRunning = true;
    }
    else{
        isRunning = false;
    }

    // initialize player and create animation cycles
    player = new Player();
    player->init("../assets/player.png", 0, (height/4)*3-50, width/8, height/4, renderer);
    player->idle_l = player->createCycle(2, 24, 32, 2, 20);
    player->idle_r = player->createCycle(1, 24, 32, 2, 20);
    player->run_l = player->createCycle(2, 24, 32, 4, 4);
    player->run_r = player->createCycle(3, 24, 32, 4, 4);
    player->setCurAnimation(player->idle_r);

    //initialize score
    score = 0;

    createSurfaces();
    startScreen(0.5);
}

// Render start screen
void Game::startScreen(double blinkRate){
    bool blink = false;
    while(!keystate[SDL_SCANCODE_RETURN] && isRunning){
        SDL_GetWindowSize(window, &width, &height);
        updateRect(&bgRect1, 0, 0, width, height);
        updateRect(&bgRect2, width, 0, width, height);
        updateRect(&messageRect, width/2 - messageRect.w/2, height/2 - messageRect.h, 600, 300);
        updateRect(&instructionRect, messageRect.x + messageRect.w/8, messageRect.y + messageRect.h, 400, 100);
        int delay = time(NULL) - lastBlink;
        if(delay > blinkRate){
            blink = !blink;
            lastBlink = time(NULL);
        }
        handleEvents();
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, bgTex1, NULL, NULL);
        SDL_RenderCopy(renderer, bgTex2, NULL, NULL);
        SDL_RenderCopy(renderer, messageTex, NULL, &messageRect);
        if(blink){
            SDL_RenderCopy(renderer, instructionTex, NULL, &instructionRect);
        }
        SDL_RenderPresent(renderer);
    }
}

//resets the player to default position and removes enemies
void Game::resetLevel(){
    score = 0;
    player->rect.x =  0;
    player->rect.y = (height/4)*3-50;

    destroyEnemies();

}

// create all background and text surfaces
void Game::createSurfaces(){
    bgTex1 = generateTexture("../assets/background1.png");
    bgTex2 = generateTexture("../assets/background1.png");
    messageTex = generateFont("../fonts/OpenSans-Regular.ttf", 36, title, {37,137,189,255});
    instructionTex = generateFont("../fonts/OpenSans-Regular.ttf", 24, "Press 'Return' to Begin", {255,255,255,255});
    scoreTex = generateFont("../fonts/OpenSans-Regular.ttf", 30, scoreChars, {255,255,255,255});
    floorTex = generateTexture("../assets/sideObject.png");
    floorTex2 = generateTexture("../assets/sideObject.png");
    updateRect(&messageRect, width/2 - messageRect.w/2, height/2 - messageRect.h, 600, 300);
    updateRect(&instructionRect, messageRect.x + messageRect.w/8, messageRect.y + messageRect.h, 400, 100);
    updateRect(&scoreRect, (width/8)*7, 0, 50, 60);
    updateRect(&floorRect, 0, height-50, width, 100);
    updateRect(&floorRect2, width, height-50, width, 100);
}

// Take care of events while game engine is running
void Game::handleEvents(){
    // look for quit
    SDL_Event event;
    SDL_PollEvent(&event);

    switch(event.type){
        case SDL_QUIT:
            isRunning = false;
            break;
        default:
            break;
    }
}

// Move update object positions and score
void Game::update(){
    checkPause();
    updateTime();
    int w = width;
    int h = height;
    SDL_GetWindowSize(window, &width, &height);
    if(w != width or h != height){
        updateRect(&bgRect1, 0, 0, width, height);
        updateRect(&bgRect2, width, 0, width, height);
    }
    else{
        updateRect(&bgRect1, bgRect1.x, bgRect1.y, width, height);
        updateRect(&bgRect2, bgRect2.x, bgRect2.y, width, height);
    }

    scoreString = std::to_string(score);
	scoreChars = scoreString.c_str();

    scoreTex = generateFont("../fonts/OpenSans-Regular.ttf", 24, scoreChars, {255,255,255,255});
    updateRect(&scoreRect, (width/8)*7, 0, 50, 50);
    if(score >= 10) addEnemy(ENEMY_RATE);
    player->updatePosition(width, height, PLAYER_SPEED, keystate, &bgRect1, &bgRect2, &floorRect, &floorRect2);
    updateObjects(ENEMY_SPEED);
}

// Add objects to renderer
void Game::render(){
    // render layer by layer
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, bgTex1, NULL, &bgRect1);
    SDL_RenderCopy(renderer, bgTex2, NULL, &bgRect2);
    SDL_RenderCopy(renderer, scoreTex, NULL, &scoreRect);
    SDL_RenderCopy(renderer, floorTex, NULL, &floorRect);
    SDL_RenderCopy(renderer, floorTex2, NULL, &floorRect2);
    player->render();
    renderEnemies();
    SDL_RenderPresent(renderer);
}

// Clean after game exit
void Game::clean(){
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();    
}

// render all enemies
void Game::renderEnemies(){
    for(auto it = enemies.begin(); it != enemies.end(); it++){
        (*it)->render();
    }
}

//clear existing enemy objects
void Game::destroyEnemies(){
    enemies.clear();
}

// populate deque with a new enemy after 'rate' seconds
void Game::addEnemy(float rate)
{
    float delay = ((float)(clock() - lastEnemyTime)) / CLOCKS_PER_SEC;
    if (delay > rate)
    {
        lastEnemyTime = clock();
        srand(rand());
        Obstacle *enemy = new Obstacle();
        int pickOne = rand() % 3;

        //select a random enemy to add
        switch (pickOne)
        {
        case 0:
            enemy->init("../assets/spikes.png", width, height - 100, width / 5, height / 8, renderer);
            break;
        case 1:
            enemy->init("../assets/triangle1.png", width, rand() % (height - 150) + 150, width / 20, height / 10, renderer);
            break;
        case 2:
            enemy->init("../assets/onionMonster.png", width, rand() % (height - 150) + 150, width / 15, height / 5, renderer);
            break;
        }

        enemies.push_back(enemy);
    }
}

//updates the score to increment by one every second
//checks if player has moved in the past second, if not change the animation to idle
int playerX;
void Game::updateTime(){
    playerX = player->rect.x;
    if(time(0) > t){
        if(playerX == player->rect.x){
            player->setCurAnimation(player->idle_r);
        }
        score++;
        t = time(0);
        
    }

}

// update enemy positions and check for collisions
void Game::updateObjects(int enemy_speed)
{
    for (auto it = enemies.begin(); it != enemies.end();)
    {
        bool collision = false;
        //if there is a collision, reset the level
        if (detectCollision(&(player->rect), &((*it)->rect)))
        {
            collision = true;
            resetLevel();
            break;
        }
        else
        {   
            //if there is no collisions, check if obstacles are being overcome and update enemy positions
            if (!collision && it != enemies.end())
            {
                overcomeObstacle(&(player->rect), &((*it)->rect));
                (*it)->updatePosition(width, height, enemy_speed);
                while (enemies.size() > 0 && enemies.front()->rect.x + enemies.front()->rect.w <= 0)
                {
                    enemies.pop_front();
                }
                ++it;
            }
        }
    }
}

// generate SDL texture given file name
SDL_Texture *Game::generateTexture(const char *filename){
    SDL_Surface *surface = IMG_Load(filename);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

// generate SDL font given file name and required parameters
SDL_Texture *Game::generateFont(const char *filename, int fontSize, const char *text, SDL_Color color){
    TTF_Font *font = TTF_OpenFont(filename, fontSize); 
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, color); 
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface); 
    SDL_FreeSurface(surface);
    return texture;
}

// update SDL_Rect attributes
void Game::updateRect(SDL_Rect *rect, int x, int y, int w, int h){
    rect->x = x;
    rect->y = y;
    rect->w = w;
    rect->h = h;
}

//Pause the game by pressing 'P' and resume by pressing 'R'
void Game::checkPause(){
    if (keystate[SDL_SCANCODE_P]){
        SDL_Texture *pauseTex = generateFont("../fonts/OpenSans-Regular.ttf", 36, "Game Paused", {37,137,189,255});
        SDL_Texture *cueTex = generateFont("../fonts/OpenSans-Regular.ttf", 24, "Press 'R' to Resume", {255,255,255,255});
        while(!keystate[SDL_SCANCODE_R] and isRunning){
            handleEvents();
            int w = width;
            int h = height;
            SDL_GetWindowSize(window, &width, &height);

            updateRect(&messageRect, width/2 - messageRect.w/2, height/2 - messageRect.h, 600, 300);
            updateRect(&instructionRect, messageRect.x + messageRect.w/8, messageRect.y + messageRect.h, 400, 100);
            if(w != width or h != height){
                updateRect(&bgRect1, 0, 0, width, height);
                updateRect(&bgRect2, width, 0, width, height);
            }
            else{
                updateRect(&bgRect1, bgRect1.x, bgRect1.y, width, height);
                updateRect(&bgRect2, bgRect2.x, bgRect2.y, width, height);
            }
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, bgTex1, NULL, &bgRect1);
            SDL_RenderCopy(renderer, bgTex2, NULL, &bgRect2);
            player->render();
            for(auto it = enemies.begin(); it != enemies.end(); it++){
                (*it)->render();
            }
            SDL_RenderCopy(renderer, pauseTex, NULL, &messageRect);
            SDL_RenderCopy(renderer, cueTex, NULL, &instructionRect);
            SDL_RenderPresent(renderer);    
        }
    }
}

// Game running loop
void Game::run(){

    //Limits the frame rate to 60 FPS for consistency across different OS
    int frameTime;
    uint32_t frameStart;
    const int FPS = 60;
    const int frameDelay = 1000 / FPS;

    while(isRunning){
        frameStart = SDL_GetTicks();
        
        handleEvents();
        update();
        render();

        frameTime = SDL_GetTicks() - frameStart;
        if (frameDelay > frameTime){
            SDL_Delay(frameDelay - frameTime);
        }
    }
    clean();
}


// function for calculating collisions between bounding boxes
bool Game::detectCollision(SDL_Rect *r1, SDL_Rect *r2){
    int width1 = r1->x + r1->w;
    int height1 = r1->y + r1->h;
    int width2 = r2->x + r2->w;
    int height2 = r2->y + r2->h;

    if (r1->x < width2 && r2->x < width1 && r1->y < height2 && r2->y < height1){
        return true;
    }
    return false;
}

// function for checking how an obstacle is overcome and adjusting point value
void Game::overcomeObstacle(SDL_Rect *r1, SDL_Rect *r2){

    if(std::abs(r1->x - r2->x) <= 5){
        //overcame obstacle without going over
        if(r1->y > r2->y){
            score += 15;
        }
        //overcame obstacle by jumping over
        if(r1->y < r2->y){
            score += 50;
        }
    }
}