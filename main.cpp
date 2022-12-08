//#define DEBUG
#define delay(A) SDL_Delay(A)

#include <SDL.h>
#include <stdio.h>
#define DEFAULT_PIXEL_SIZE 7

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
SDL_Renderer* gRenderer = nullptr;

typedef uint32_t color_type;

const color_type RED = 0xFF0000;
const color_type YELLOW = 0xFFFF00;

int size = DEFAULT_PIXEL_SIZE;
int space = 1;
int x = space;
int y = space;
uint16_t gridSX = ((SCREEN_WIDTH - size) / (size + space)) + space;
uint16_t gridSY = ((SCREEN_HEIGHT - size) / (size + space)) + space;

struct gamegrid {
	bool cursor;
	bool lock;
	color_type color;
};


enum {
	DOWN,
	UP,
	LEFT,
	RIGHT
};

bool move[] { false, false, false, false };

uint16_t playerPosX = 0;
uint16_t playerPosY = 0;

void movement(gamegrid **game_grid)
{
#ifdef DEBUG
	printf("move: %d %d %d %d\n", move[UP], move[DOWN], move[LEFT], move[RIGHT]);
#endif

	// reset pixel under cursor
	game_grid[playerPosX][playerPosY].cursor = false;

	// adjust position
	if (move[UP]) {
		if (!playerPosY)
			playerPosY = gridSY - 1;
		else
			playerPosY--;
	}
	if (move[DOWN]) {
		playerPosY++;
	}
	if (move[LEFT]) {
		if (!playerPosX)
			playerPosX = gridSX - 1;
		else
			playerPosX--;
	}

	if (move[RIGHT]) {
		playerPosX++;
	}

	// reset key flags
	for (auto& i:move) {
		i = false;
	}

	// loop around
	if (playerPosX >= gridSX)
		playerPosX = 0;

	if (playerPosY >= gridSY)
		playerPosY = 0;

	// set pixel under cursor
	game_grid[playerPosX][playerPosY].cursor = true;

#ifdef DEBUG
	printf("playerPosX: %d\tplayerPosY: %d\n", playerPosX, playerPosY);
#endif

	// redraw
	SDL_RenderPresent(gRenderer);
}

void lock()
{
}

bool drawn = false;

void setColor(color_type color)
{
	uint8_t red, green, blue;
	red = color >> 16;
	green = color >> 8;
	blue = color;
	SDL_SetRenderDrawColor(gRenderer, red, green, blue, 0xff);
}


void draw(gamegrid **game_grid, SDL_Rect **grid, SDL_Renderer *gRenderer)
{
	for (size_t y = 0; y < gridSY; y++) {
		for (size_t x = 0; x < gridSX; x++) {
			if (!game_grid[x][y].cursor && !game_grid[x][y].lock)
				//if (x != playerPosX || y != playerPosY)
				setColor(RED);
			//SDL_SetRenderDrawColor(gRenderer, 0xff, 0x00, 0x00, 0xff);
			else
				setColor(YELLOW);
			//SDL_SetRenderDrawColor(gRenderer, 0xff, 0xff, 0x00, 0xff);
			SDL_RenderFillRect(gRenderer, &grid[x][y]);
		}
	}
}

void loop(gamegrid **game_grid, SDL_Renderer *gRenderer, SDL_Rect **grid, bool& quit)
{
	// SDL event
	SDL_Event e;

	// main loop TODO: move
	while (SDL_PollEvent(&e) != 0) {

		movement(game_grid);
		draw(game_grid, grid, gRenderer);

		if (!drawn) {
			SDL_RenderPresent(gRenderer);
			drawn = true;
		}


		if (e.type == SDL_QUIT) {
			quit = true;
		}

		if (e.type == SDL_KEYDOWN) {
			// refresh
			switch (e.key.keysym.sym) {
				case SDLK_q: quit = true; break;
				case SDLK_j: move[DOWN] = true; break;
				case SDLK_k: move[UP] = true; break;
				case SDLK_h: move[LEFT] = true; break;
				case SDLK_l: move[RIGHT] = true; break;
				case SDLK_SPACE:
					     game_grid[playerPosX][playerPosY].lock = !game_grid[playerPosX][playerPosY].lock;
					     break;
				default: break;
			}
		}
	}
}

void sdlbullshit(SDL_Window *window)
{
	// create window
	window = SDL_CreateWindow(
			"Game Grid",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			SCREEN_WIDTH, SCREEN_HEIGHT,
			SDL_WINDOW_SHOWN
			);
	if (window == NULL) {
		printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
	}
	else {
		// renderer
		gRenderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );

		SDL_RenderClear(gRenderer);

		// display grid
		// goddamn!
		//SDL_Rect grid[gridSX][gridSY];

		SDL_Rect **grid;

		grid = new SDL_Rect *[gridSX];

		for (size_t x = 0; x < gridSX; x++) {
			grid[x] = new SDL_Rect[gridSY];
		}

		// game grid (remembers pixels)
		gamegrid **game_grid;//[gridSX][gridSY];

		// init game grid (need to pass to funcs, that's why...)
		game_grid = new gamegrid *[gridSX];
		for (size_t x = 0; x < gridSX; x++) {
			game_grid[x] = new gamegrid[gridSY];
		}

		for (size_t y = 0; y < gridSY; y++) {
			for (size_t x = 0; x < gridSX; x++) {
				game_grid[x][y].cursor = false;
				game_grid[x][y].lock = false;
			}
		}

		// init display grid
		for (size_t y = 0; y < gridSY; y++) {
			for (size_t x = 0; x < gridSX; x++) {
				grid[x][y].y = y + (y * size) + space;
				grid[x][y].x = x + (x * size) + space;
				grid[x][y].h = grid[x][y].w = size;
			}
		}

		// quit flag
		bool quit = false;

		while(!quit)
			loop(game_grid, gRenderer, grid, quit);

		for (size_t x = 0; x < gridSX; x++) {
			delete[] game_grid[x];
			delete[] grid[x];
		}

		delete[] game_grid;
		delete[] grid;
	}
}

int main( int argc, char* args[] )
{
	// dynamic pixel size
	if (argc > 1) {
		int firstarg = atoi(args[1]);
		if (firstarg > 0) {
			size = firstarg;
			gridSX = ((SCREEN_WIDTH - size) / (size + space)) + space;
			gridSY = ((SCREEN_HEIGHT - size) / (size + space)) + space;
		}
		else
			printf("first arg should be a number greater than zero\n");
	}
	// window
	SDL_Window* window = NULL;

	// init SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0 ) {
		printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
	}
	else
		sdlbullshit(window);

	//Destroy window
	SDL_DestroyWindow( window );

	//Quit SDL subsystems
	SDL_Quit();

	return 0;
}
