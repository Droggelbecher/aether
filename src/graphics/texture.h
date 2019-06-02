
#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <optional>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../coordinate.h"
#include "../utils.h"
#include "screen.h"

/**
 * An owning wrapper for SDL_Texture* with some convenience methods for construction and rendering.
 */
class Texture {
public:

	enum Flags {
		NONE,
		CLEAR,
		BLACK
	};

	Texture(): _sdl_texture(nullptr) {
	}

	operator bool() {
		return _sdl_texture;
	}

	Texture(SDL_Renderer *r, Coordi size) {
		_sdl_texture = SDL_CreateTexture(
			r, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
			size.x(), size.y()
		);
		SDL_SetTextureBlendMode(_sdl_texture, SDL_BLENDMODE_BLEND);
	}

	Texture(SDL_Renderer *r, const char *filename) {
		_sdl_texture = IMG_LoadTexture(r, filename);
		if(!_sdl_texture) {
			throw std::runtime_error(fmt::format("Error loading {}", filename));
		}
		SDL_SetTextureBlendMode(_sdl_texture, SDL_BLENDMODE_BLEND);
	}

	Texture(Texture&& other) noexcept {
		_sdl_texture = other._sdl_texture;
		other._sdl_texture = nullptr;
		_scale = other._scale;
	}
	
	Texture& operator=(Texture&& other) noexcept {
		_free_texture();
		_sdl_texture = other._sdl_texture;
		other._sdl_texture = nullptr;
		_scale = other._scale;
		return *this;
	}

	Texture& operator=(const Texture& other) = delete;

	Texture clone(SDL_Renderer *sdl_renderer) const {
		Texture r;
		r._scale = _scale;

		if(_sdl_texture && sdl_renderer) {
			int w, h;
			SDL_QueryTexture(_sdl_texture, nullptr, nullptr, &w, &h);
			r._sdl_texture = SDL_CreateTexture(
				sdl_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
				w, h
			);

			auto prev_target = SDL_GetRenderTarget(sdl_renderer);
			SDL_SetRenderDrawBlendMode(sdl_renderer, SDL_BLENDMODE_NONE);
			SDL_SetRenderTarget(sdl_renderer, r._sdl_texture);
			SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 0, 0);
			SDL_RenderFillRect(sdl_renderer, nullptr);
			SDL_RenderCopy(sdl_renderer, _sdl_texture, nullptr, nullptr);
			SDL_SetRenderTarget(sdl_renderer, prev_target);
			SDL_SetTextureBlendMode(r._sdl_texture, SDL_BLENDMODE_BLEND);
		}
		return r;
	}

	virtual ~Texture() {
		_free_texture();
	}

	Texture& adopt(SDL_Texture* tex) {
		_free_texture();
		_sdl_texture = tex;
		return *this;
	}

	SDL_Texture* sdl_texture() {
		return _sdl_texture;
	}

	Texture& set_scale(double scale) & {
		_scale = scale;
		return *this;
	}

	Texture&& set_scale(double scale) && {
		_scale = scale;
		return std::move(*this);
	}

	double scale() { return _scale; }

	Coord2i size() const {
		int w, h;
		SDL_QueryTexture(_sdl_texture, nullptr, nullptr, &w, &h);
		return {
			static_cast<int>(w * _scale),
			static_cast<int>(h * _scale)
		};
	}

	void set_alpha(uint8_t alpha) {
		SDL_SetTextureAlphaMod(_sdl_texture, alpha);
	}

	void render(SDL_Renderer* r,
			std::optional<Coord2i> target = std::nullopt,
			std::optional<Coord2i> target_size = std::nullopt
	) const {
		if(!_sdl_texture) {
			return;
		}

		SDL_Rect tgt { 0, 0, 0, 0 };
		if(target) {
			tgt.x = target.value()[0];
			tgt.y = target.value()[1];
		}
		if(target_size) {
			tgt.w = target_size.value()[0];
			tgt.h = target_size.value()[1];
		}
		else {
			Coordi sz = size();
			tgt.w = sz[0];
			tgt.h = sz[1];
		}

		SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
		SDL_RenderCopy(r, _sdl_texture, nullptr, &tgt);
	}

	/**
	 * Set the target of passed renderer to this texture and
	 * prepare the texture for rendering.
	 * When returned ScopeGuard is destructed, reset renderer to previous state.
	 * 
	 * Use like:
	 * auto renderer = ...
	 * {
	 *   auto guard = texture.begin_render_onto(renderer);
	 *   // render onto texture
	 * }
	 * // renderer is back to previous target here
	 */
	ScopeGuard begin_render_onto(SDL_Renderer* r, Flags flags = NONE) {
		auto prev_target = SDL_GetRenderTarget(r);

		SDL_SetRenderTarget(r, _sdl_texture);
		SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_NONE);
		if(flags & BLACK) {
			SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
			SDL_RenderFillRect(r, nullptr);
		}
		if(flags & CLEAR) {
			SDL_SetRenderDrawColor(r, 0, 0, 0, 0);
			SDL_RenderFillRect(r, nullptr);
		}
		
		return {[=]{ SDL_SetRenderTarget(r, prev_target); }};
	}

private:

	void _free_texture() {
		if(_sdl_texture) {
			SDL_DestroyTexture(_sdl_texture);
			_sdl_texture = nullptr;
		}
	}

	SDL_Texture *_sdl_texture;
	//SDL_Renderer *_sdl_renderer = nullptr; // ONLY tracked for operator=, can we avoid this somehow?
	double _scale = 1.;
};

#endif // _TEXTURE_H_

