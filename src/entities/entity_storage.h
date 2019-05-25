
#ifndef _ENTITY_STORAGE_H_
#define _ENTITY_STORAGE_H_

#include <vector>
#include "graphics_component.h"
#include "physics_component.h"

/**
 * Central storage for all game entities and components.
 * Entity: Objects like units, buildings, asteroids, force fields, etc...
 * Component: Behavioral aspects of such an entity like Physics, Graphics, etc...
 *
 * For better cache-locality in the hot loop we want to store components of the same type densely
 * packed, as a value-type.
 * That means we need to sacrifice virtual inhertitance as slicing would make it hard to use.
 * Instead, component classes will hold function pointers where necessary.
 * (Lastly unavoidable if behavior for entities can be different and chosen at runtime, be it
 * through vtable or manual fptrs).
 */
class EntityStorage {
	/*
	 * Assumptions in implementation:
	 * - Most entities have a (useful) component of each type, e.g. it is not
	 *   necessary to move a lot of "inactive" elements out of the way to
	 *   conserve cache locality
	 * - Fragmentation due to a lot of entities being created and *then* a lot
	 *   of older ones being removed is not an issue (other order is fine).
	 * Thus, each entity can be an index (=entity id) into a couple of parallel
	 * vectors, that will not change as long as the entity exists
	 *
	 * If these assumptions do not hold anymore (eg. lead to performance
	 * problems), consider using eg. a std::map to translate entity IDs to
	 * array indices such that we can reorder them.
	 */

	public:

		using id_t = size_t;

		/*
		 * Handy wrapper around entity ID for accessing its components.
		 * Also serves as iterator over entities.
		 */
		struct Entity {
			id_t id;
			EntityStorage &storage;

			GraphicsComponent& graphics() { return storage.graphics(id); }
			PhysicsComponent& physics() { return storage.physics(id); }

			Entity& operator*() { return *this; }
			bool operator==(const Entity& other) const { return id == other.id; }
			bool operator!=(const Entity& other) const { return !(*this == other); }
			Entity& operator++() { id++; return *this; }
			Entity operator++(int) { return { id++, storage }; }
		};
		using EntityIterator = Entity;

		GraphicsComponent& graphics(id_t id) { return _graphics[id]; }
		PhysicsComponent& physics(id_t id) { return _physics[id]; }
		
		Entity make_entity() {
			emplace_back();
			return { size() - 1, *this };
		}

		size_t size() {
			return _physics.size();
		}

		EntityIterator begin() {
			return { 0, *this };
		}

		EntityIterator end() {
			return { size(), *this };
		}

	private:
		void emplace_back() {
			_physics.emplace_back();
			_graphics.emplace_back();
		}

		std::vector<PhysicsComponent> _physics;
		std::vector<GraphicsComponent> _graphics;

};

#endif // _ENTITY_STORAGE_H_

