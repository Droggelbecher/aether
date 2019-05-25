
#include <SDL2/SDL.h>

#include "sdl_utils.h"
#include "graphics/screen.h"
#include "game_loop.h"
#include "entities/entity_storage.h"

#include "graphics/map_view.h"

void init_sdl() {
	auto r = SDL_Init(SDL_INIT_VIDEO);
	check_sdl(r != -1, "initialize");
} // init_sdl()


void run_game() {

	init_sdl();
	
	EntityStorage storage;
	Screen screen;

	MapView map_view { screen.sdl_renderer(), { 600, 400 }, storage };
	UserInterface ui { &map_view };
	GameLoop loop { screen, ui };

	auto asteroid = storage.make_entity();
	asteroid.graphics().texture() = { screen.sdl_renderer(), "resources/asteroid.png" };
	asteroid.graphics().texture().set_scale(0.2);
	asteroid.physics().set_position({ 0, 0, 1.0 });

	loop.run();
}


int main(int argc, char* argv[]) {

	run_game();

	return 0;
}

