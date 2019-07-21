
#include <vector>
#include <chrono>

#include <fmt/printf.h>
#include <SDL2/SDL.h>

#include "sdl_utils.h"
#include "graphics/screen.h"
#include "game_loop.h"
#include "entities/entity_storage.h"

#include "graphics/map_view.h"
#include "map/map_layer.h"
#include "map/map.h"
#include "ui/terminal.h"

#include "lua_interface.h"

void init_sdl() {
	auto r = SDL_Init(SDL_INIT_VIDEO);
	check_sdl(r != -1, "initialize SDL");

	r = IMG_Init(IMG_INIT_PNG);
	check_sdl(r == IMG_INIT_PNG, "initialize IMG");

	r = TTF_Init();
	check_sdl(r != -1, "initialize TTF");
} // init_sdl()


void run_game() {

	using namespace std::chrono_literals;

	init_sdl();
	
	EntityStorage storage;
	Screen screen;

	// Create map

	Hex map_size = { 21, 21 };
	auto map = Map { map_size, storage };
	auto center = map_size / 2;

	/* TBD: Maybe play around with ranges-v3?
	map.coordinates()
		| ranges::view::filter([](const Hex& h) { return hex_distance(h, center) <= center.q(); })
		| ranges::view::for_each([map&](const Hex& h) { map.make_walkable(h); });

	In this particular case a good ole for loop seems more readable tho:
	*/

	for(auto h: map) {
		if(hex_distance(h, center) <= center.q()) {
			map.set_walkable(h);
		}
	}


	MapView map_view {
		screen.sdl_renderer(), { 1200, 800 },
		map, storage
	};

	Terminal terminal {
		{ 0, 0 }, { 1200, 60 },
		"resources/fonts/SourceCodePro/SourceCodePro-Regular.ttf",
		14
	};


	UserInterface ui { &map_view, &terminal };
	GameLoop loop { screen, ui, storage };

	init_lua(storage, loop);

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
			Texture{ screen.sdl_renderer(), "resources/asteroid2.png" }
			.set_scale(.4)
		)
		.set_hovering(.02, 3s, 0s);
	asteroid.physics().set_position({0, 2, .6});

	asteroid = storage.make_entity();
	asteroid.graphics()
		.set_texture(
			Texture{ screen.sdl_renderer(), "resources/asteroid1.png" }
			.set_scale(.2)
		)
		.set_hovering(.02, 3s, 2s);
	asteroid.physics().set_position({0, 1, .4});

	asteroid.clone(screen.sdl_renderer()).physics().set_position({1, 0, .6});
	asteroid.clone(screen.sdl_renderer()).physics().set_position({2, 0, .6});
	asteroid.clone(screen.sdl_renderer()).physics().set_position({3, 0, .6});

	loop.run();
}


int main(int argc, char* argv[]) {

	run_game();

	return 0;
}

