
#ifndef _GAME_LOOP_H_
#define _GAME_LOOP_H_

#include <SDL2/SDL.h>
#include <fmt/printf.h>

#include "commands.h"
#include "user_interface.h"
#include "entities/entity_storage.h"
#include "graphics/screen.h"

class GameLoop {
public:

	GameLoop(Screen& screen, UserInterface& ui)
		: _screen(screen), _user_interface(ui) {
	}

	void run() {

		while(!_stop) {
			_process_events();

			_render_everything();
			// TODO: regulate frame rate, update things in the world, etc..
		} // while !stop
	} // run()

	void stop() {
		_stop = true;
	}

private:

	void _process_events() {
		SDL_Event event;

		while(SDL_PollEvent(&event)) {
			switch(event.type) {
				case SDL_MOUSEMOTION:
					if(event.motion.state & SDL_BUTTON_LMASK) {
						_emit(DragCommand { event.motion.xrel, event.motion.yrel });
					}
					break;

				case SDL_MOUSEWHEEL:
					_emit(ZoomCommand { event.wheel.y });
					break;

				case SDL_QUIT:
					fmt::print("Goodbye!\n");
					_stop = true;
					return;
			}
		} // while event
	}

	void _render_everything() {
		auto guard = _screen.begin_render_onto();
		_user_interface.render(_screen.sdl_renderer());
	}


	void _emit(const Command& command) {
		_user_interface.process_command(command);
	}

	bool _stop = false;

	Screen &_screen;
	UserInterface &_user_interface;
};

#endif // _GAME_LOOP_H_
