
#ifndef _UI_ELEMENT_H_
#define _UI_ELEMENT_H_

#include <SDL2/SDL.h>
#include "commands.h"

class UIElement {
	public:
		virtual bool process_command(const Command&) { return false; }
		virtual void render(SDL_Renderer *) {};
};

#endif // _UI_ELEMENT_H_

