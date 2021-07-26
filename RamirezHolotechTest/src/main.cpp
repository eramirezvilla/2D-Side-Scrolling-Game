#include "game.h"

//main function to initiate the game
int main(void){
	Game *game = new Game();
	game->init("Ramirez Run", 1080, 600, false);
	game->run();
	return 0;
}
