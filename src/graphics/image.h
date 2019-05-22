
#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <string>
#include <SDL.h>
#include <SDL_image.h>

// TODO: Unify with Texture
// TODO: should this be a string_view?
class Image {
public:
	Image(Screen& screen, const char* filename)
		: _screen(screen) {
		//auto surface = IMG_Load(filename);
		//_sdl_texture = SDL_CreateTextureFromSurface(_screen.sdl_renderer(), surface);
		_sdl_texture = IMG_LoadTexture(_screen.sdl_renderer(), filename);
		//SDL_FreeSurface(surface);
	}

	virtual ~Image() {
		SDL_DestroyTexture(_sdl_texture);
	}

	void render_to_screen() {
		auto r = _screen.sdl_renderer();
		SDL_SetRenderTarget(r, nullptr);
		SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
		SDL_RenderCopy(r, _sdl_texture, nullptr, nullptr);
	}


private:
	SDL_Texture *_sdl_texture;
	Screen& _screen;
};

#endif // _IMAGE_H_
