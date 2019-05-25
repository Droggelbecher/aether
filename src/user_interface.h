
#ifndef _USER_INTERFACE_H_
#define _USER_INTERFACE_H_

#include <variant>

#include "commands.h"
#include "ui_element.h"

class UserInterface {

public:

	UserInterface(std::initializer_list<UIElement*> ui_elements)
		: _elements { ui_elements }
	{
	}

	bool process_command(const Command& command) {
		for(auto& e: _elements) {
			if(e->process_command(command)) {
				return true;
			}
		}
		return false;
	}

	void render(SDL_Renderer *r) {
		for(auto& e: _elements) {
			e->render(r);
		}
	}

private:
	std::vector<UIElement*> _elements;
};

#endif // _USER_INTERFACE_H_

