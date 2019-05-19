
#include <cstdint>
#include <fmt/format.h>
#include <SDL.h>

#include "intensity_map.h"
#include "screen.h"
#include "colormaps.h"

class IntensityMapView {
	public:

		IntensityMapView(const IntensityMap& im, Screen& screen)
			: _intensity_map(im), _screen(screen) {

			_pixel_format = SDL_AllocFormat(SDL_PIXELFORMAT_ARGB8888);
			_texture = SDL_CreateTexture(
				&screen.sdl_renderer(), _pixel_format->format,
				SDL_TEXTUREACCESS_STREAMING,
				im.size().x, im.size().y
			);
		}

		void update() {
			/*
			auto renderer = &_screen.sdl_renderer();
			SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
			SDL_RenderClear(renderer);
			SDL_Rect r = { 50, 50, 50, 50 };
			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			SDL_RenderDrawRect(renderer, &r);
			*/
			_update_texture();
			_screen.render(_texture);
		}

	private:

		void _update_texture() {
			uint32_t *pixels;
			int pitch;
			uint32_t format;
			int w, h;
			SDL_QueryTexture(_texture, &format, nullptr, &w, &h);
			SDL_LockTexture(_texture, nullptr, reinterpret_cast<void**>(&pixels), &pitch);
			fmt::print("Pitch={0} format={1} w={2} h={3}", pitch, format, w, h);

			auto sz = _intensity_map.size();

			for(Coordi::T ix = 0; ix < sz.x; ix++) {
				for(Coordi::T iy = 0; iy < sz.y; iy++) {
					auto v = _intensity_map(ix, iy);
					pixels[iy * (pitch / sizeof(uint32_t)) + ix] =
						test(v, _pixel_format);
						//SDL_MapRGBA(_pixel_format, 200, v * 255, v * 255, 255);
				}
			}

			SDL_UnlockTexture(_texture);
		}

		const IntensityMap& _intensity_map;
		SDL_Texture *_texture;
		//SDL_Renderer *_renderer;
		Screen &_screen;
		SDL_PixelFormat *_pixel_format;
};

