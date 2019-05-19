
#include <SDL.h>

#include "sdl_utils.h"
#include "screen.h"
//#include "intensity_map.h"
#include "hex_intensity_map_view.h"
//#include "intensity_map_perlin.h"
//#include "intensity_map_noise.h"
//#include "intensity_map_blur.h"
//#include "xtensor/xarray.hpp"

void init_sdl() {
	auto r = SDL_Init(SDL_INIT_VIDEO);
	check_sdl(r != -1, "initialize");
} // init_sdl()

int main(int argc, char* argv[]) {

	init_sdl();

	//auto screen = new Screen();
	Screen screen;

	// Generate intensity map

	//auto im = generate_subdivision({1025, 1025});
	//auto imv = IntensityMapView(im, screen);
	auto imv = HexIntensityMapView(screen, {1000, 1000});

	SDL_Delay(100);

	screen.render_start();
	//imv.update();

	imv.render();

	screen.render_end();

	
	SDL_Delay(5000);
	
	//screen.update();
	//SDL_Delay(5000);

	return 0;
}

