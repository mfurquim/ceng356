#include "game.h"

int main (int argc, char* args[])
{
	Game game;
	if (!game.init())
		fprintf(stderr, "Failed to initialize!\n");

	game.loop();
	game.close();

	return 0;
}
