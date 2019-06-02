
#ifndef _UI_ELEMENT_H_
#define _UI_ELEMENT_H_

#include <chrono>
#include <SDL2/SDL.h>
#include "../commands.h"

class UIElement {
	public:

		// TODO: Remove this, UI Elements should be the ones
		// that turn SDL events into more abstract commands (move _process_events into UI elem)
		virtual bool process_event(const SDL_Event&) { return false; }
		//virtual bool process_command(const Command&) { return false; }

		virtual void render(SDL_Renderer *) {};

		virtual void update(std::chrono::nanoseconds) { };
};

#endif // _UI_ELEMENT_H_

