#ifndef GAME_H_
#define GAME_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>
#include "timer.h"

class Game
{
public:
	//Starts up SDL, and creates a window
	bool init();

	//Game loop
	void loop();

	//Frees media and shuts down SDL
	void close();

private:
	//nHD Screen dimensions 360p
	static const int SCREEN_WIDTH	= 640;
	static const int SCREEN_HEIGHT	= 360;
	
	//Frames per second
	static const int FRAME_PER_SECOND = 60;

	//Frequency of the game 60Hz
	float FRAME_MILISECOND;

	//The window where the render will occur
	SDL_Window* gWindow = NULL;

	//OpenGL context
	SDL_GLContext gContext;

	//Exit flag
	bool exit_game;

	//Event handler
	SDL_Event event;

	//Timer to update screen
	Timer frame_time;

	//Initialize functions
	bool initGL();
	void initialize_variables();

	//Game loop functions
	void update_time_step();
	void receive_network_data();
	void handle_events();
	void run_ai();
	void run_physics();
	void update();
	void send_network_data();
	void render();

	//Close functions

	//Render functions
	bool check_if_skip();
	void render_cube();
};

#endif