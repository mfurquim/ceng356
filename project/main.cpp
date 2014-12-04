#include "game.h"

int main (int argc, char* args[])
{
	Game game;
	if (!game.init())
		fprintf(stderr, "Failed to initialize!\n");

	game.loop();
	game.shut_down();

	return 0;
}
