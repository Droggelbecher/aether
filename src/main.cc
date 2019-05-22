
#include <SDL.h>

#include "sdl_utils.h"
#include "graphics/screen.h"
#include "graphics/hex_intensity_map_view.h"
#include "game_loop.h"

void init_sdl() {
	auto r = SDL_Init(SDL_INIT_VIDEO);
	check_sdl(r != -1, "initialize");
} // init_sdl()

int main(int argc, char* argv[]) {

	init_sdl();

	// Generate intensity map
	//auto imv = HexIntensityMapView(screen, {1000, 1000});
	SDL_Delay(100);

	//screen.render_start();
	//imv.render();
	//screen.render_end();

	//SDL_Delay(5000);
	

	UserInterface ui;

	GameLoop(ui).run();
	
	return 0;
}

