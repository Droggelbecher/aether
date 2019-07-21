
#ifndef _USER_INTERFACE_H_
#define _USER_INTERFACE_H_

#include <variant>

#include "../commands.h"
#include "ui_element.h"
#include "../graphics/map_view.h"

class UserInterface {

public:

	UserInterface(std::initializer_list<UIElement*> ui_elements)
		: _elements { ui_elements }
	{
	}

	bool process_event(const SDL_Event& event) {
		for(auto& e: _elements) {
			if(e->process_event(event)) {
				return true;
			}
		}
		return false;
	}

	void update(std::chrono::nanoseconds dt) {
		for(auto& e: _elements) {
			e->update(dt);
		}
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

