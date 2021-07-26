#ifndef game_h
#define game_h

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <deque>
#include <ctime>
#include "object.h"

class Game{
public:
	Game();
	~Game();
	void init(const char *title, int width, int height, bool fullscreen);
	void run();
	int score= 0;

protected:
	void handleEvents();
	void update();
	void checkPause();
	void render();
	void clean();
	void createSurfaces();
	void updateRect(SDL_Rect *rect, int x, int y, int w, int h);
	bool running(){ return isRunning; }
	void startScreen(double blinkRate);
	bool detectCollision(SDL_Rect *r1, SDL_Rect *r2);
	void overcomeObstacle(SDL_Rect *r1, SDL_Rect *r2);
	void addEnemy(float rate);
	void updateObjects(int enemy_speed);
	void renderEnemies();
	void destroyEnemies();
	void updateTime();
	long t = time(0);
	void resetLevel();

	SDL_Texture *generateTexture(const char *filename);
	SDL_Texture *generateFont(const char *filename, int fontSize, const char *text, SDL_Color color);

	const Uint8 *keystate = SDL_GetKeyboardState(NULL);
	clock_t lastEnemyTime = clock();
	double lastBlink = time(NULL);

	clock_t scoreTimer = clock();

	int width; 
	int height; 
	bool isRunning;
	const char *title;
	Player *player; 
	std::deque<Obstacle *> enemies;
	std::string scoreString = std::to_string(score);
	char const *scoreChars = scoreString.c_str();

	SDL_Window *window; 
	SDL_Renderer *renderer; 
	SDL_Texture *bgTex1, *bgTex2, *messageTex, *instructionTex, *scoreTex, *floorTex, *floorTex2;
	SDL_Rect bgRect1, bgRect2, messageRect, instructionRect, scoreRect, floorRect, floorRect2; 
};

#endif