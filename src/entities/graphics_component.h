
#ifndef _GRAPHICS_COMPONENT_H_
#define _GRAPHICS_COMPONENT_H_

#include "graphics/texture.h"

class GraphicsComponent {

	public:
		GraphicsComponent() {
		}

		Texture& texture() { return _texture; }

	private:
		Texture _texture;
};

#endif // _GRAPHICS_COMPONENT_H_

