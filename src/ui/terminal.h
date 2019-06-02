
#ifndef _TERMINAL_H_
#define _TERMINAL_H_

#include <string_view>
#include <SDL2/SDL_ttf.h>
#include <fmt/format.h>

#include "ui_element.h"
#include "../coordinate.h"
#include "../sdl_utils.h"

class Terminal: public UIElement {
	public:

		Terminal(Coord2i position, Coord2i size, const char* path, int fontsize)
			: _position(position), _size(size) {
			_prompt = "Æther> ";
			if(path) {
				load_font(path, fontsize);
			}
		}

		virtual ~Terminal() {
			_free_font();
			_free_texture();
		}

		void render(SDL_Renderer *r) override {
			_update_texture(r);

			int w, h;
			SDL_QueryTexture(_texture, nullptr, nullptr, &w, &h);
			SDL_Rect tgt {
				_position.x(), _position.y(),
				w, h
			};
			SDL_RenderCopy(r, _texture, nullptr, &tgt);
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

		void _free_texture() {
			if(_texture) {
				SDL_DestroyTexture(_texture);
				_texture = nullptr;
			}
		}

		void _update_texture(SDL_Renderer *renderer) {
			if(!_dirty) {
				return;
			}
			auto surface = TTF_RenderUTF8_Shaded(
					_font, (_prompt + _line).c_str(),
					_text_color, _background_color
			);
			_free_texture();
			_texture = SDL_CreateTextureFromSurface(renderer, surface);
			SDL_FreeSurface(surface);
			_dirty = false;
		}

		Coord2i _position;
		Coord2i _size;
		TTF_Font *_font = nullptr;
		std::string _prompt;
		std::string _line;
		SDL_Color _text_color = { 0xff, 0xff, 0xff };
		SDL_Color _background_color = { 0x20, 0x20, 0x20 };
		SDL_Texture *_texture = nullptr;
		bool _dirty = true;

};


#endif // _TERMINAL_H_

