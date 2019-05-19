
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
		
	}

	virtual ~Texture() {
		//SDL_FreeTexture(_sdl_texture); // TODO
		_sdl_texture = nullptr;
	}

	SDL_Texture* sdl_texture() {
		return _sdl_texture;
	}

	void render_to_screen() {
		auto r = _screen.sdl_renderer();
		SDL_SetRenderTarget(r, nullptr); //_screen.window());
		SDL_Rect target { 0, 0, _size.x, _size.y };
		SDL_RenderCopy(r, _sdl_texture, nullptr, &target);
	}

	SDL_Renderer* render_start() { // TODO: make this return Resource<SDL_Renderer*>?
		auto r = _screen.sdl_renderer();
		SDL_SetRenderTarget(r, _sdl_texture);
		return r;
	}

private:
	SDL_Texture *_sdl_texture;
	//SDL_Renderer *_renderer;
	Screen& _screen;
	Coordi _size;
};

#endif // _TEXTURE_H_

