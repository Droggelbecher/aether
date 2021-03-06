
#ifndef _SCREEN_H_
#define _SCREEN_H_

#include <fmt/printf.h>
#include <SDL2/SDL.h>

#include "../sdl_utils.h"
#include "../utils.h"

class Screen {

	public:
		Screen() {
			int w = 1200;
			int h = 800;

			_window = SDL_CreateWindow(
				"AEther",
				SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
				w, h,
				SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
			);
			check_sdl(_window != nullptr, "window creation");
			//SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "bilinear");

			_renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_ACCELERATED);
			//_renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_TARGETTEXTURE);
		}

		ScopeGuard begin_render_onto() {
			SDL_SetRenderTarget(_renderer, nullptr);
			SDL_SetRenderDrawBlendMode(_renderer, SDL_BLENDMODE_NONE);
			SDL_SetRenderDrawColor(_renderer, 10, 10, 10, 255);
			SDL_RenderClear(_renderer);

			return {[=]{
				SDL_SetRenderTarget(_renderer, nullptr);
				SDL_RenderPresent(_renderer); // actually display on the screen
			}};
		}

		void render(SDL_Texture* texture) {
			SDL_RenderCopy(_renderer, texture, nullptr, nullptr);
		}

		SDL_Renderer* sdl_renderer() {
			return _renderer;
		}

	private:
		SDL_Window *_window;
		SDL_Renderer *_renderer;
};

#endif // _SCREEN_H_

