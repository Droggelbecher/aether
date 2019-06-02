
#ifndef _TERMINAL_H_
#define _TERMINAL_H_

#include <string_view>
#include <SDL2/SDL_ttf.h>
#include <fmt/format.h>

#include "ui_element.h"
#include "../coordinate.h"
#include "../sdl_utils.h"
#include "../graphics/texture.h"

class Terminal: public UIElement {
	public:

		Terminal(Coord2i position, Coord2i size, const char* path, int fontsize)
			: _position(position), _size(size) {
			_prompt = "Æther ~> ";
			if(path) {
				load_font(path, fontsize);
			}
		}

		virtual ~Terminal() {
			_free_font();
		}

		void render(SDL_Renderer *r) override {
			_update_texture(r);

			_texture.render(r, std::nullopt, _texture.size());
		}

		void load_font(const char* path, int size) {
			_free_font();
			_font = TTF_OpenFont(path, size);
			check_sdl(_font, fmt::format("loading font '{}'", path));
		}

		bool process_event(const SDL_Event& event) override {
			switch(event.type) {
				case SDL_TEXTINPUT:
					_line += event.text.text;
					_dirty = true;
					return true;
					break;

				case SDL_KEYDOWN:
					switch(event.key.keysym.sym) {
						case SDLK_BACKSPACE:
							if(_line.size()) {
								_line.pop_back();
								_dirty = true;
							}
							break;

						case SDLK_RETURN:
						case SDLK_RETURN2:
							fmt::print("Would run: {}\n", _line);
							_line = "";
							_dirty = true;
							break;
					}
					return true;
					break;
			}
			return false;
		}

	private:

		void _free_font() {
			if(_font) {
				TTF_CloseFont(_font);
				_font = nullptr;
			}
		}

		//void _free_texture() {
			//if(_texture) {
				//SDL_DestroyTexture(_texture);
				//_texture = nullptr;
			//}
		//}

		Texture _text(SDL_Renderer *renderer, const char *s, SDL_Color fg, SDL_Color bg) {
			Texture r;

			auto surface = TTF_RenderUTF8_Shaded(_font, s, fg, bg);
			r.adopt(SDL_CreateTextureFromSurface(renderer, surface));
			SDL_FreeSurface(surface);
			return r;
		}

		void _update_texture(SDL_Renderer *renderer) {
			if(!_dirty) {
				return;
			}

			if(!_texture) {
				_texture = { renderer, _size };
				//_texture.set_alpha(200);
			}

			auto _guard = _texture.begin_render_onto(renderer, Texture::CLEAR);

			// Backdrop
			// Fill with semi-transparent background
			SDL_SetRenderDrawColor(renderer, _background_color.r, _background_color.g, _background_color.b, 255);
			SDL_RenderClear(renderer);

			// Render Prompt

			auto prompt = _text(renderer, _prompt.c_str(), _prompt_color, _background_color);
			prompt.render(renderer, Coord2i { 10, 10 } );

			auto line = _text(renderer, _line.c_str(), _line_color, _background_color);
			line.render(renderer, Coord2i { prompt.size().x() + 10, 10 });

			_dirty = false;
		}

		Coord2i _position;
		Coord2i _size;
		TTF_Font *_font = nullptr;
		std::string _prompt;
		std::string _line;
		SDL_Color _prompt_color = { 0x80, 0xa0, 0xf0 };
		SDL_Color _line_color = { 0xff, 0xff, 0xff };
		SDL_Color _background_color = { 0x20, 0x20, 0x20 };
		Texture _texture;
		bool _dirty = true;

};


#endif // _TERMINAL_H_

