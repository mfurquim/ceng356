#include "game.h"

#include <iostream>

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
	if( error != GL_NO_ERROR )
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

	return success;
}

void
Game::initialize_variables()
{
	FRAME_MILISECOND = 1000 / FRAME_PER_SECOND;
	exit_game = false;

	return ;
}

void
Game::loop()
{
	while (!exit_game)
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
Game::close()
{
	//Disable text input
	SDL_StopTextInput();

	//Destroy window	
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

void
Game::update_time_step()
{
    frame_time.start();
}

void
Game::receive_network_data()
{

}

void
Game::handle_events()
{
	while (SDL_PollEvent (&event) != 0)
	{
		//User requests quit
		if (event.type == SDL_QUIT )
		{
			exit_game = true;
		}
		//Handle keypress with current mouse position
		else if (event.type == SDL_TEXTINPUT)
		{
			switch (event.text.text[0])
			{
			case 'q':
				exit_game = true;
				break;

			default:
				break;
			}
		}
	}
}

void
Game::run_ai()
{

}

void
Game::run_physics()
{

}

void
Game::update()
{

}

void
Game::send_network_data()
{

}

void
Game::render()
{
	if (check_if_skip())
		return ;

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	render_cube();

	//Update screen
	SDL_GL_SwapWindow (gWindow);
}

bool
Game::check_if_skip()
{
	if (frame_time.get_ticks() < FRAME_MILISECOND) {
		frame_time.wait_diff (FRAME_MILISECOND);
		return false;
	}

	return true;
}

void
Game::render_cube()
{
	float rotation = 0;
	glRotatef(rotation+1, 1, 0, 0);
	glRotatef(rotation+2, 0, 1, 0);
	glRotatef(rotation, 0, 0, 1);
	rotation = rotation + 0.0000005;
	//Multi-colored side - FRONT
	glBegin(GL_QUADS);
	glColor3f(   1.0,  0.0,  0.0 );
	glVertex3f(  0.5, -0.5, -0.5 );      // P1 is red
	glColor3f(   0.0,  1.0,  0.0 );
	glVertex3f(  0.5,  0.5, -0.5 );      // P2 is green
	glColor3f(   0.0,  0.0,  1.0 );
	glVertex3f( -0.5,  0.5, -0.5 );      // P3 is blue
	glColor3f(   1.0,  0.0,  1.0 );
	glVertex3f( -0.5, -0.5, -0.5 );      // P4 is purple
	glEnd();

	// White side - BACK
	glBegin(GL_QUADS);
	glColor3f(   1.0,  1.0, 1.0 );
	glVertex3f(  0.5, -0.5, 0.5 );
	glVertex3f(  0.5,  0.5, 0.5 );
	glVertex3f( -0.5,  0.5, 0.5 );
	glVertex3f( -0.5, -0.5, 0.5 );
	glEnd();

	// Purple side - RIGHT
	glBegin(GL_QUADS);
	glColor3f(  1.0,  0.0,  1.0 );
	glVertex3f( 0.5, -0.5, -0.5 );
	glVertex3f( 0.5,  0.5, -0.5 );
	glVertex3f( 0.5,  0.5,  0.5 );
	glVertex3f( 0.5, -0.5,  0.5 );
	glEnd();
	 
	// Green side - LEFT
	glBegin(GL_QUADS);
	glColor3f(   0.0,  1.0,  0.0 );
	glVertex3f( -0.5, -0.5,  0.5 );
	glVertex3f( -0.5,  0.5,  0.5 );
	glVertex3f( -0.5,  0.5, -0.5 );
	glVertex3f( -0.5, -0.5, -0.5 );
	glEnd();
	 
	// Blue side - TOP
	glBegin(GL_QUADS);
	glColor3f(   0.0,  0.0,  1.0 );
	glVertex3f(  0.5,  0.5,  0.5 );
	glVertex3f(  0.5,  0.5, -0.5 );
	glVertex3f( -0.5,  0.5, -0.5 );
	glVertex3f( -0.5,  0.5,  0.5 );
	glEnd();
	 
	// Red side - BOTTOM
	glBegin(GL_QUADS);
	glColor3f(   1.0,  0.0,  0.0 );
	glVertex3f(  0.5, -0.5, -0.5 );
	glVertex3f(  0.5, -0.5,  0.5 );
	glVertex3f( -0.5, -0.5,  0.5 );
	glVertex3f( -0.5, -0.5, -0.5 );
	glEnd();
}
