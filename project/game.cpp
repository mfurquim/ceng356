#include "game.h"

#include <iostream>
#include <cmath>
#include <sys/socket.h>

using namespace std;

void
Game::update_time_step()
{
	frame_time.start();

	return ;
}

void
Game::receive_network_data()
{
	nread = read(sfd, &buf_float, sizeof(float));
	if (nread == -1) {
		perror("read");
		exit(EXIT_FAILURE);
	}

//	printf("Received %ld bytes: %f\n", (long) nread, (float) buf_float);
	position = buf_float;
	return ;
}

void
Game::sdl_handle_events()
{
	while (SDL_PollEvent (&event) != 0)
	{
		//User requests quit
		if (event.type == SDL_QUIT )
		{
			exit_game = true;
		}
		//Handle keypress
		else if (event.type == SDL_TEXTINPUT)
		{
			switch (event.text.text[0])
			{
			case 'q':
				exit_game = true;
				break;

			case 'a':
				gforce_x += 1;
				if (gforce_x >= 360)
					gforce_x -= 360;
				break;

			case 's':
				gforce_y += 1;
				if (gforce_y >= 360)
					gforce_y -= 360;
				break;

			case 'd':
				gforce_z += 1;
				if (gforce_z >= 360)
					gforce_z -= 360;
				break;

			case 'e':
				//Cycle through projection scales
				if( gProjectionScale == 1.f )
				{
					//Zoom out
					gProjectionScale = 2.f;
				}
				else if( gProjectionScale == 2.f )
				{
					//Zoom in
					gProjectionScale = 0.5f;
				}
				else if( gProjectionScale == 0.5f )
				{
					//Regular zoom
					gProjectionScale = 1.f;
				}
				break;
				
			default:
				break;
			}
		}
	}

	return ;
}

void
Game::wiimote_event(struct wiimote_t* wm)
{
//	printf("\n\n--- EVENT [id %i] ---\n", wm->unid);

	/* if a button is pressed, report it 
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_A))
		printf("A pressed\n");

	if (IS_PRESSED(wm, WIIMOTE_BUTTON_B))
		printf("B pressed\n");

	if (IS_PRESSED(wm, WIIMOTE_BUTTON_UP))
		printf("UP pressed\n");

	if (IS_PRESSED(wm, WIIMOTE_BUTTON_DOWN))
		printf("DOWN pressed\n");

	if (IS_PRESSED(wm, WIIMOTE_BUTTON_LEFT))
		printf("LEFT pressed\n");

	if (IS_PRESSED(wm, WIIMOTE_BUTTON_RIGHT))
		printf("RIGHT pressed\n");

	if (IS_PRESSED(wm, WIIMOTE_BUTTON_MINUS))
		printf("MINUS pressed\n");

	if (IS_PRESSED(wm, WIIMOTE_BUTTON_PLUS))
		printf("PLUS pressed\n");

	if (IS_PRESSED(wm, WIIMOTE_BUTTON_ONE))
		printf("ONE pressed\n");

	if (IS_PRESSED(wm, WIIMOTE_BUTTON_TWO))
		printf("TWO pressed\n");

	if (IS_PRESSED(wm, WIIMOTE_BUTTON_HOME))
		printf("HOME pressed\n");
	*/
	/* if the accelerometer is turned on then print angles */
	if (WIIUSE_USING_ACC(wm)) {
		/*
		printf("wiimote roll  		= %f [%f]\n", wm->orient.roll, wm->orient.a_roll);
		printf("wiimote pitch 		= %f [%f]\n", wm->orient.pitch, wm->orient.a_pitch);
		printf("wiimote yaw   		= %f\n", wm->orient.yaw);
		printf("wiimote gravity		= (%4.2f,%4.2f,%4.2f)\n", wm->gforce.x, wm->gforce.y, wm->gforce.z);
		printf("wiimote accel		= (%d,%d,%d)\n", wm->accel.x, wm->accel.y, wm->accel.z);
		*/
		prev_accel_x = wm->gforce.x;
		prev_accel_y = wm->gforce.y;
		prev_accel_z = wm->gforce.z;

		if (prev_accel_x < 0)
			prev_accel_x *= -1;

		if (prev_accel_y < 0)
			prev_accel_y *= -1;

		if (prev_accel_z < 0)
			prev_accel_z *= -1;

		gforce_x += prev_accel_x;
		gforce_y += prev_accel_y;
		gforce_z += prev_accel_z;

		if (gforce_x >= 360)
			gforce_x -= 360;

		if (gforce_y >= 360)
			gforce_y -= 360;

		if (gforce_z >= 360)
			gforce_z -= 360;

		/*
		rotation = abs(prev_accel_x - wm->accel.x) + abs(prev_accel_y - wm->accel.y) + abs(prev_accel_z - wm->accel.z);
		
		gforce_x = wm->gforce.x * wm->orient.roll;
		gforce_y = wm->gforce.y * wm->orient.pitch;
		gforce_z = wm->gforce.z;

		printf("cube_rotation: %f (%f, %f, %f)\n", rotation, gforce_x, gforce_y, gforce_z);

		prev_accel_x = wm->accel.x;
		prev_accel_y = wm->accel.y;
		prev_accel_z = wm->accel.z;
		*/
	}

	return ;
}

void
Game::wiimote_ctrl_status(struct wiimote_t* wm)
{
	printf("\n\n--- CONTROLLER STATUS [wiimote id %i] ---\n", wm->unid);

	printf("attachment:      %i\n", wm->exp.type);
	printf("speaker:         %i\n", WIIUSE_USING_SPEAKER(wm));
	printf("ir:              %i\n", WIIUSE_USING_IR(wm));
	printf("leds:            %i %i %i %i\n", WIIUSE_IS_LED_SET(wm, 1), WIIUSE_IS_LED_SET(wm, 2), WIIUSE_IS_LED_SET(wm, 3), WIIUSE_IS_LED_SET(wm, 4));
	printf("battery:         %f %%\n", wm->battery_level);

	return ;
}

void
Game::wiimote_disconnect(wiimote* wm)
{
	printf("\n\n--- DISCONNECTED [wiimote id %i] ---\n", wm->unid);

	return ;
}

void
Game::wiimote_handle_events()
{
	if (wiiuse_poll(wiimotes, MAX_WIIMOTES)) {
	int i = 0;
	for (; i < MAX_WIIMOTES; ++i) {
		switch (wiimotes[i]->event) {
			case WIIUSE_EVENT:
				wiimote_event(wiimotes[i]);
				break;

			case WIIUSE_STATUS:
				wiimote_ctrl_status(wiimotes[i]);
				break;

			case WIIUSE_DISCONNECT:
			case WIIUSE_UNEXPECTED_DISCONNECT:
				wiimote_disconnect(wiimotes[i]);
				break;

			default:
				break;
			}
		}
	}

	return ;
}

void
Game::handle_events()
{
	sdl_handle_events();
	wiimote_handle_events();

	return ;
}

void
Game::run_ai()
{
	return ;
}

void
Game::run_physics()
{
	return ;
}

void
Game::update()
{
	float temp;
	temp = (prev_accel_x + prev_accel_y + prev_accel_z);
	if (player_number == 2)
		temp *= -1;
//	printf("Oh Yeah! [%d]%f\n", player_number, temp);
	position += temp;
	send = temp;

//	printf("Player %d [%4.2f]\n", player_number, number);

	if (position >= SCREEN_WIDTH/2 || position <= -SCREEN_WIDTH/2)
		exit_game = true;

	return ;
}

void
Game::send_network_data()
{
	if ((size_t) write(sfd, &send, sizeof(float)) != len) {
		fprintf(stderr, "partial/failed write\n");
		exit(EXIT_FAILURE);
	}
	
	return ;
}
/*
void
Game::render_spots()
{
	glPushMatrix();

	glBegin(GL_QUADS);
	glColor3f(   1.0,  0.0,  0.0 );
	glVertex2f( -1.0, -1.0 );
	glVertex2f(  0.5,  0.5 );
	glVertex2f( -0.5,  0.5 );
	glVertex2f( -0.5, -0.5 );
	glEnd();

	glPopMatrix();

}
*/

void
Game::render_background()
{
	glPushMatrix();
	glLoadIdentity();

	glBegin(GL_QUADS);

	glColor3f( (float) position/SCREEN_WIDTH,  0.0,  (float) (SCREEN_WIDTH - position)/SCREEN_WIDTH );
	glVertex2f(	SCREEN_WIDTH,	0.0 );
	glVertex2f(	SCREEN_WIDTH,	SCREEN_HEIGHT );
	
	glColor3f( (float) (SCREEN_WIDTH - position)/SCREEN_WIDTH,  0.0,  (float) position/SCREEN_WIDTH );
	glVertex2f(	0.0,			SCREEN_HEIGHT );
	glVertex2f(	0.0,			0.0 );
	glEnd();

	glPopMatrix();

	return ;
}

void
Game::render_cube()
{
	glPushMatrix();
	glLoadIdentity();
	glTranslatef( SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f, -60.f );
	glTranslatef(position, 0, 0);
	glScalef (0.3, 0.3, 0.3);


	glRotatef(gforce_x, rotation, 0, 0);
	glRotatef(gforce_y, 0, rotation, 0);
	glRotatef(gforce_z, 0, 0, rotation);


	//Multi-colored side - FRONT
	glBegin(GL_QUADS);
	glColor3f(   1.0,  0.0,  0.0 );
	glVertex3f(  50.0, -50.0, -50.0 );      // P1 is red
	glColor3f(   0.0,  1.0,  0.0 );
	glVertex3f(  50.0,  50.0, -50.0 );      // P2 is green
	glColor3f(   0.0,  0.0,  1.0 );
	glVertex3f( -50.0,  50.0, -50.0 );      // P3 is blue
	glColor3f(   1.0,  0.0,  1.0 );
	glVertex3f( -50.0, -50.0, -50.0 );      // P4 is purple
	glEnd();

	// White side - BACK
	glBegin(GL_QUADS);
	glColor3f(   1.0,  1.0, 1.0 );
	glVertex3f(  50.0, -50.0, 50.0 );
	glVertex3f(  50.0,  50.0, 50.0 );
	glVertex3f( -50.0,  50.0, 50.0 );
	glVertex3f( -50.0, -50.0, 50.0 );
	glEnd();

	// Purple side - RIGHT
	glBegin(GL_QUADS);
	glColor3f(  1.0,  0.0,  1.0 );
	glVertex3f( 50.0, -50.0, -50.0 );
	glVertex3f( 50.0,  50.0, -50.0 );
	glVertex3f( 50.0,  50.0,  50.0 );
	glVertex3f( 50.0, -50.0,  50.0 );
	glEnd();
	 
	// Green side - LEFT
	glBegin(GL_QUADS);
	glColor3f(   0.0,  1.0,  0.0 );
	glVertex3f( -50.0, -50.0,  50.0 );
	glVertex3f( -50.0,  50.0,  50.0 );
	glVertex3f( -50.0,  50.0, -50.0 );
	glVertex3f( -50.0, -50.0, -50.0 );
	glEnd();
	 
	// Blue side - TOP
	glBegin(GL_QUADS);
	glColor3f(   0.0,  0.0,  1.0 );
	glVertex3f(  50.0,  50.0,  50.0 );
	glVertex3f(  50.0,  50.0, -50.0 );
	glVertex3f( -50.0,  50.0, -50.0 );
	glVertex3f( -50.0,  50.0,  50.0 );
	glEnd();
	 
	// Red side - BOTTOM
	glBegin(GL_QUADS);
	glColor3f(   1.0,  0.0,  0.0 );
	glVertex3f(  50.0, -50.0, -50.0 );
	glVertex3f(  50.0, -50.0,  50.0 );
	glVertex3f( -50.0, -50.0,  50.0 );
	glVertex3f( -50.0, -50.0, -50.0 );
	glEnd();
	glPopMatrix();
}

void
Game::render()
{

	if (check_if_skip())
		return ;

	//Update projection matrix
//	glMatrixMode( GL_PROJECTION );
//	glLoadIdentity();
//	glOrtho( 0.0, SCREEN_WIDTH * gProjectionScale, SCREEN_HEIGHT * gProjectionScale, 0.0, 100.0, -100.0 );

	glMatrixMode(GL_MODELVIEW);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glLoadIdentity();

	render_background();
//	render_spots();
	render_cube();

	//Update screen
	SDL_GL_SwapWindow (gWindow);
}

bool
Game::check_if_skip()
{
	if (frame_time.get_ticks() < FRAME_MILISECOND) {
		frame_time.wait_diff(FRAME_MILISECOND);
		return false;
	}

	return true;
}

short any_wiimote_connected(wiimote** wm, int wiimotes)
{
	int i;
	if (!wm)
		return 0;

	for (i = 0; i < wiimotes; i++)
		if (wm[i] && WIIMOTE_IS_CONNECTED(wm[i]))
			return 1;

	return 0;
}

int
Game::initialize_connection()
{
	/* Obtain address(es) matching host/port */

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
	hints.ai_flags = 0;
	hints.ai_protocol = 0;          /* Any protocol */

	s = getaddrinfo("127.0.0.1", "9966", &hints, &result);

//	s = getaddrinfo("142.104.91.69", "9966", &hints, &result);
	if (s != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
		exit(EXIT_FAILURE);
	}

	/* getaddrinfo() returns a list of address structures.
	   Try each address until we successfully connect(2).
	   If socket(2) (or connect(2)) fails, we (close the socket
	   and) try the next address. */

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sfd = socket(rp->ai_family, rp->ai_socktype,
					 rp->ai_protocol);
		if (sfd == -1)
			continue;

		if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
			break;                  /* Success */

		close(sfd);
	}

	if (rp == NULL) {               /* No address succeeded */
		fprintf(stderr, "Could not connect\n");
		exit(EXIT_FAILURE);
	}

	freeaddrinfo(result);           /* No longer needed */

	number = 0;
	if ((size_t) write(sfd, &number, len) != len) {
		fprintf(stderr, "partial/failed write\n");
		exit(EXIT_FAILURE);
	}

	nread = read(sfd, &buf_int, sizeof(int));
	if (nread == -1) {
		perror("read");
		exit(EXIT_FAILURE);
	}
	player_number = buf_int;

	return EXIT_SUCCESS;
}

void
Game::initialize_variables()
{
	FRAME_MILISECOND = 1000 / FRAME_PER_SECOND;
	exit_game = false;

	rotation = 1;
	gforce_x = 0;
	gforce_y = 0;
	gforce_z = 0;
	prev_accel_x = 0;
	prev_accel_y = 0;
	prev_accel_z = 0;

	position = 0;

	len = sizeof(float);

	gProjectionScale = 1.f;

	return ;
}

bool
Game::initialize_wiimote()
{
	bool success = true;
	int found = 0;
	int connected = 0;

	wiimotes = wiiuse_init(MAX_WIIMOTES);

	found = wiiuse_find(wiimotes, MAX_WIIMOTES, 5);
	if (!found)
	{
		fprintf(stderr, "No wiimotes found.\n");
		success = false;
	}
	else
	{
		connected = wiiuse_connect(wiimotes, MAX_WIIMOTES);
		if (!connected)
		{
			fprintf(stderr, "Failed to connect to any wiimote.\n");
			success = false;
		}
		else
		{
			fprintf(stderr, "Connected to %i wiimotes (of %i found).\n", connected, found);
			wiiuse_set_leds(wiimotes[0], WIIMOTE_LED_1);

			wiiuse_set_orient_threshold(wiimotes[0], 10);
			wiiuse_set_accel_threshold(wiimotes[0], 10);
			wiiuse_set_motion_plus(wiimotes[0], 1);
			wiiuse_set_ir(wiimotes[0], 1);
			wiiuse_motion_sensing(wiimotes[0], 1);

			wiiuse_rumble(wiimotes[0], 1);
			SDL_Delay (200);
			wiiuse_rumble(wiimotes[0], 0);

		}
	}


	return success;
}

bool
Game::initGL()
{
	bool success = true;
	GLenum error = GL_NO_ERROR;

	//Initialize projection matrix
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	glOrtho( 0.0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0, 100.0, -100.0 );
	//Check for error
	error = glGetError();
	if (error != GL_NO_ERROR)
	{
		fprintf(stderr, "Error initializing OpenGL! %s\n", gluErrorString (error));
		success = false;
	}

	//Initialize modelview matrix
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();

	//Check for error
	error = glGetError();
	if (error != GL_NO_ERROR)
	{
		fprintf(stderr, "Error initializing OpenGL! %s\n", gluErrorString (error));
		success = false;
	}

	//Initialize clear color
	glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	//Check for error
	error = glGetError();
	if (error != GL_NO_ERROR)
	{
		fprintf(stderr, "Error initializing OpenGL! %s\n", gluErrorString (error));
		success = false;
	}

//	printf("OpenGL version supported by this platform (%s): \n", glGetString(GL_VERSION));

	return success;
}

bool
Game::init()
{
	bool success = true;

	if (SDL_Init (SDL_INIT_VIDEO) < 0)
	{
		fprintf(stderr, "SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Using OpenGL 2.1
		SDL_GL_SetAttribute (SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute (SDL_GL_CONTEXT_MINOR_VERSION, 1);

		gWindow = SDL_CreateWindow ("CENG 356", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			fprintf(stderr, "Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create context
			gContext = SDL_GL_CreateContext (gWindow);
			if (gContext == NULL)
			{
				fprintf(stderr, "OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//User VSync
				if (SDL_GL_SetSwapInterval (1) < 0)
				{
					fprintf(stderr, "Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
					SDL_GL_SetSwapInterval (0);
				}

				//Initialize OpenGL
				if (!initGL())
				{
					fprintf(stderr, "Unable to initialize OpenGL!\n");
					success = false;
				}
			}
		}
	}

	//Enable text input
	SDL_StartTextInput();
	initialize_variables();
	initialize_connection();
	initialize_wiimote();

	return success;
}

void
Game::loop()
{
	while (!exit_game && any_wiimote_connected(wiimotes, MAX_WIIMOTES))
	{
		update_time_step();
		receive_network_data();
		handle_events();
		run_ai();
		run_physics();
		update();
		send_network_data();
		render();
	}

	return ;
}

void
Game::shut_down()
{
	wiiuse_cleanup(wiimotes, MAX_WIIMOTES);

	//Disable text input
	SDL_StopTextInput();

	//Destroy window	
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

