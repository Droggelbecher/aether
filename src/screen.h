
#ifndef _SCREEN_H_
#define _SCREEN_H_

#include <SDL.h>

class Screen {

	public:
		Screen() {
			int w = 800;
			int h = 600;

			_window = SDL_CreateWindow(
				"SubSpace 0.1",
				SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
				w, h,
				SDL_WINDOW_SHOWN
			);
			check_sdl(_window != nullptr, "window creation");
			//SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

			_renderer = SDL_CreateRenderer(_window, -1, 0);
			_texture = SDL_CreateTexture(_renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, w, h);
		}

		void render_start() {
			SDL_RenderClear(_renderer);
		}

		void render_end() {
			// copy _surface (cpu) to _texture (gpu)
			//SDL_UpdateTexture(_texture, nullptr, _surface->pixels, _surface->pitch);
			//SDL_UpdateTexture(_texture, nullptr, _surface->pixels, _surface->pitch);
			
			//SDL_RenderCopy(_renderer, _texture, nullptr, nullptr); // render the texture
			SDL_RenderPresent(_renderer); // actually display on the screen
		}

		void render(SDL_Texture* texture) {
			SDL_RenderCopy(_renderer, texture, nullptr, nullptr);
		}


		SDL_Renderer* sdl_renderer() {
			return _renderer;
		}

		//SDL_Surface* sdl_window() {
			//return _window;
		//}
		
		//SDL_Texture *sdl_texture() {
			//return _texture;
		//}

	private:
		SDL_Window *_window;
		SDL_Renderer *_renderer;
		SDL_Texture *_texture;

		SDL_Surface *_surface;
};

#endif // _SCREEN_H_

