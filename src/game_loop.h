
#ifndef _GAME_LOOP_H_
#define _GAME_LOOP_H_

#include <chrono>
#include <thread>
//#include <unistd.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <fmt/printf.h>

#include "commands.h"
#include "ui/user_interface.h"
#include "entities/entity_storage.h"
#include "graphics/screen.h"
#include "accumulator.h"

class GameLoop {
public:

	GameLoop(Screen& screen, UserInterface& ui, EntityStorage& storage)
		: _screen(screen), _user_interface(ui), _storage(storage) {
	}

	void run() {
		using namespace std;
		using namespace std::chrono;
		//using frame = duration<int32_t, ratio<1, 100>>;

		Accumulator acc(nanoseconds {5 * 1000000000ULL});
		nanoseconds dt { 0 };
		time_point<steady_clock> start = steady_clock::now();
		while(!_stop) {
			_process_events();
			_update_everything(dt);
			_render_everything();

			//this_thread::sleep_until(start + frame{ 1 });

			dt = steady_clock::now() - start;

			start = steady_clock::now(); // Frame starts here (so it includes loop overhead)
			if(acc(dt)) {
				fmt::print("FPS={}\n", 1000000000. / acc.mean().count());
			}

		} // while !stop
	} // run()

	void stop() {
		_stop = true;
	}

private:

	void _process_events() {
		SDL_Event event;

		while(SDL_PollEvent(&event)) {
			if(_user_interface.process_event(event)) {
				continue;
			}

			if(event.type == SDL_QUIT) {
				fmt::print("Goodbye!\n");
				_stop = true;
				return;
			}
		} // while event
	}

	void _update_everything(std::chrono::nanoseconds dt) {
		_storage.graphics_storage().update(dt);
		_user_interface.update(dt);
	}

	void _render_everything() {
		auto guard = _screen.begin_render_onto();
		_user_interface.render(_screen.sdl_renderer());
	}

	bool _stop = false;

	Screen &_screen;
	UserInterface &_user_interface;
	EntityStorage &_storage;
};

#endif // _GAME_LOOP_H_
