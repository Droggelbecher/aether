
#include <stdexcept>
#include <fmt/format.h>

// TODO:
/*
template<typename T>
sruct SDLResource { ... }

Usage:
Resource<SDL_Renderer*> make_renderer() {
	auto r = SDL_Create_renderer(_window, -1, 0),
	return SDLResource<SDL_Renderer*>(
		r,
		[]{ SDL_FreeRenderer(r); }
	);
}

...

{
	auto r = make_renderer();
	SDL_RenderCopy(r.get(), ...);
	// renderer gets freed automatically here
}
*/

void check_sdl(bool condition, const char*s) {
	if(!condition) {
		throw std::runtime_error(fmt::format("SDL Error during {0}: {1}", s, SDL_GetError()));
	}
}

