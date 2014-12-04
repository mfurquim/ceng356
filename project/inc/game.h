#ifndef GAME_H_
#define GAME_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>
#include <GL/gl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "timer.h"
#include "wiiuse.h"

#define BUF_SIZE 500

class Game
{
public:
	//Starts up SDL, creates a window, connects with a wiimote
	bool init();

	//Game loop
	void loop();

	//Frees media, shuts down SDL, and disconects the wiimote
	void shut_down();

private:

	int player_number;

	GLfloat gProjectionScale;

	wiimote** wiimotes;
	static const int MAX_WIIMOTES = 4;
	float rotation;
	float prev_accel_x;
	float prev_accel_y;
	float prev_accel_z;
	float gforce_x;
	float gforce_y;
	float gforce_z;

	float position;
	
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int sfd, s, j;
	size_t len;
	ssize_t nread;
	char buf[BUF_SIZE];
	float number;
	float buf_float;
	int buf_int;
	float send;

	//nHD Screen dimensions 360p
	static const int SCREEN_WIDTH	= 640;
	static const int SCREEN_HEIGHT	= 360;

	//Frames per second
	static const int FRAME_PER_SECOND = 10;

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
	bool initialize_wiimote();
	int initialize_connection();

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
	void render_background();

	//Handle event functions
	void sdl_handle_events();
	void wiimote_handle_events();
	void wiimote_event (struct wiimote_t*);
	void wiimote_ctrl_status (struct wiimote_t*);
	void wiimote_disconnect (wiimote*);

};

#endif