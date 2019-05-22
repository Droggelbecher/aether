
#ifndef _USER_INTERFACE_H_
#define _USER_INTERFACE_H_

#include "graphics/image.h"
#include "graphics/hex_intensity_map_view.h"

class UserInterface {

public:

	UserInterface()
		: _himv{ _screen, {1000, 1000} },
		  _backdrop{ _screen, "resources/backdrop.jpg" }
	{
	}

	bool process_command(const Command& command) {
		if(command.type == Command::DRAG) {
			_himv.move(command.drag.xrel, command.drag.yrel);
			return true;
		}

		return false;
	}

	void render() {
		_screen.render_start();
		_backdrop.render_to_screen(); // TODO: more consistent method naming
		_himv.render();
		_screen.render_end();
	}

private:
	Screen _screen;
	HexIntensityMapView _himv;
	Image _backdrop;
};

#endif // _USER_INTERFACE_H_

