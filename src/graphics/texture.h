
#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include "screen.h"

class Texture {
public:
	Texture(Screen& screen, Coordi size)
		: _sdl_texture(
				SDL_CreateTexture(
					screen.sdl_renderer(),
					SDL_PIXELFORMAT_RGBA8888,
					SDL_TEXTUREACCESS_TARGET,
					size.x, size.y
				)
		),
		_screen(screen),
		_size(size)
	{
		
		SDL_SetTextureBlendMode(_sdl_texture, SDL_BLENDMODE_BLEND);
	}

	virtual ~Texture() {
		SDL_DestroyTexture(_sdl_texture); // TODO
		_sdl_texture = nullptr;
	}

	SDL_Texture* sdl_texture() {
		return _sdl_texture;
	}

	void render_to_screen() {
		auto r = _screen.sdl_renderer();
		SDL_SetRenderTarget(r, nullptr); //_screen.window());
		SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
		SDL_Rect target { 0, 0, _size.x, _size.y };
		SDL_RenderCopy(r, _sdl_texture, nullptr, &target);
	}

	SDL_Renderer* render_start() { // TODO: make this return Resource<SDL_Renderer*>?
		auto r = _screen.sdl_renderer();
		SDL_SetRenderTarget(r, _sdl_texture);
		SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
		SDL_SetRenderDrawColor(r, 255, 0, 255, 0); // "invisible pink"
		SDL_RenderFillRect(r, nullptr);
		return r;
	}

private:
	SDL_Texture *_sdl_texture;
	Screen& _screen;
	Coordi _size;
};

#endif // _TEXTURE_H_

