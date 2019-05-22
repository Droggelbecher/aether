
#ifndef _GAME_LOOP_H_
#define _GAME_LOOP_H_

#include <SDL.h>
#include <fmt/printf.h>

#include "commands.h"
#include "user_interface.h"

class GameLoop {
public:

	GameLoop(UserInterface& ui)
		: _user_interface(ui) {
	}

	void run() {

		while(!_stop) {
			_process_events();

			_user_interface.render();

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
						_emit_command(drag(event.motion.xrel, event.motion.yrel));
						//fmt::print("moved! {0} {1}\n", event.motion.xrel, event.motion.yrel);
						//_intensity_map_view.move(event.motion.xrel, event.motion.yrel);
					}
					break;

				case SDL_QUIT:
					fmt::print("Goodbye!\n");
					_stop = true;
					return;
					//break;
			}
		} // while event
	}

	void _emit_command(const Command& command) {
		_user_interface.process_command(command);
	}

	bool _stop = false;

	UserInterface &_user_interface;
};

#endif // _GAME_LOOP_H_
