
#ifndef _ENTITY_STORAGE_H_
#define _ENTITY_STORAGE_H_

#include <vector>
#include <cstdint>

#include "graphics_storage.h"
#include "physics_storage.h"

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
		static constexpr id_t no_id = -1;

		/*
		 * Handy wrapper around entity ID for accessing its components.
		 * Also serves as iterator over entities.
		 */
		struct Entity {
			id_t id;
			EntityStorage &storage;

			GraphicsComponent graphics() { return storage.graphics(id); }
			PhysicsComponent physics() { return storage.physics(id); }

			Entity clone(SDL_Renderer *renderer) {
				auto e = storage.make_entity();
				e.graphics().copy_from(graphics(), renderer);
				e.physics().copy_from(physics());
				return e;
			}

			Entity& operator=(const Entity& other) { id = other.id; return *this; }
			Entity& operator*() { return *this; }
			bool operator==(const Entity& other) const { return id == other.id; }
			bool operator!=(const Entity& other) const { return !(*this == other); }
			Entity& operator++() { id++; return *this; }
			Entity operator++(int) { return { id++, storage }; }
		};
		using EntityIterator = Entity;

		struct EntityCreated {
			id_t id;
		};

		EntityStorage() {
		}

		EntityStorage(const EntityStorage&) = delete;
		void operator=(const EntityStorage&) = delete;

		GraphicsComponent graphics(id_t id) { return _graphics_storage[id]; }
		PhysicsComponent physics(id_t id) { return _physics_storage[id]; }

		
		Entity make_entity() {
			_graphics_storage.make_component();
			_physics_storage.make_component();

			auto id = size() - 1;
			_entity_created.notify({ id });
			return { id, *this };
		}

		Entity operator[](id_t id) {
			return { id, *this };
		}

		size_t size() {
			return _graphics_storage.size();
		}

		EntityIterator begin() {
			return { 0, *this };
		}

		EntityIterator end() {
			return { size(), *this };
		}

		GraphicsStorage& graphics_storage() {
			return _graphics_storage;
		}

		PhysicsStorage& physics_storage() {
			return _physics_storage;
		}

		Observable<EntityCreated>& entity_created() {
			return _entity_created;
		}

	private:

		GraphicsStorage _graphics_storage;
		PhysicsStorage _physics_storage;
		Observable<EntityCreated> _entity_created;
};

#endif // _ENTITY_STORAGE_H_

