
#ifndef _SDL_UTILS_H_
#define _SDL_UTILS_H_

#include <stdexcept>
#include <string_view>
#include <fmt/format.h>

#include <SDL2/SDL.h>

void check_sdl(bool condition, std::string_view s) {
	if(!condition) {
		throw std::runtime_error(fmt::format("SDL Error during {0}: {1}", s, SDL_GetError()));
	}
}

#endif // _SDL_UTILS_H_

