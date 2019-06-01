
#include <vector>
#include <chrono>

#include <fmt/printf.h>
#include <SDL2/SDL.h>
//#include <xtensor/xarray.hpp>

#include "sdl_utils.h"
#include "graphics/screen.h"
#include "game_loop.h"
#include "entities/entity_storage.h"

#include "graphics/map_view.h"
#include "map/map_layer.h"

void init_sdl() {
	auto r = SDL_Init(SDL_INIT_VIDEO);
	check_sdl(r != -1, "initialize");
} // init_sdl()


void run_game() {

	using namespace std::chrono_literals;

	init_sdl();
	
	EntityStorage storage;
	Screen screen;

	auto walkable = make_hexagon<bool>(20, true);


	MapView map_view {
		screen.sdl_renderer(), { 1200, 800 },
		*walkable,
		storage
	};

	UserInterface ui { map_view };
	GameLoop loop { screen, ui, storage };

	auto asteroid = storage.make_entity();
	asteroid.graphics()
		.set_texture(
			Texture{ screen.sdl_renderer(), "resources/asteroid.png" }
			.set_scale(.2)
		)
		.set_hovering(.02, 3s, 1s);
	asteroid.physics().set_position({0, 0, .5});

	asteroid = storage.make_entity();
	asteroid.graphics()
		.set_texture(
			Texture{ screen.sdl_renderer(), "resources/asteroid1.png" }
			.set_scale(.2)
		)
		.set_hovering(.02, 3s, 2s);
	asteroid.physics().set_position({0, 1, .4});

	asteroid = storage.make_entity();
	asteroid.graphics()
		.set_texture(
			Texture{ screen.sdl_renderer(), "resources/asteroid2.png" }
			.set_scale(.4)
		)
		.set_hovering(.02, 3s, 0s);
	asteroid.physics().set_position({0, 2, .6});

	asteroid.clone(screen.sdl_renderer()).physics().set_position({1, 0, .6});
	asteroid.clone(screen.sdl_renderer()).physics().set_position({2, 0, .6});
	asteroid.clone(screen.sdl_renderer()).physics().set_position({3, 0, .6});

	loop.run();
}


int main(int argc, char* argv[]) {

	run_game();

	return 0;
}

