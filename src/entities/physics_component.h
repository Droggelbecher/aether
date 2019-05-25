
#ifndef _PHYSICS_COMPONENT_H_
#define _PHYSICS_COMPONENT_H_

#include "map/hex_coordinate.h"

class PhysicsComponent {
	
	public:
		PhysicsComponent() {
		}

		Hex position() const { return _position; }
		void set_position(Hex p) { _position = p; }

	private:
		Hex _position { 0, 0 };
		size_t _radius { 1 };
};

#endif // _PHYSICS_COMPONENT_H_

