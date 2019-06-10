
#ifndef _PHYSICS_STORAGE_H_
#define _PHYSICS_STORAGE_H_

#include <cstdint>
#include <vector>
#include "../map/hex_coordinate.h"
#include "../observable.h"

class PhysicsStorage {

	public:
		using id_t = std::size_t;

		struct EntityMoved {
			id_t id;
			Hex from;
			Hex to;
		};

		struct PhysicsComponent {
			public:
				PhysicsComponent(PhysicsStorage& storage, id_t id)
					: _storage(storage), _id(id) {
				}

				PhysicsComponent(const PhysicsComponent& other)
					: _storage(other._storage), _id(other._id) {
				}

				PhysicsComponent& operator=(const PhysicsComponent& other) = delete;

				PhysicsComponent& copy_from(const PhysicsComponent& other) {
					set_position(other.position());
					return *this;
				}

				PhysicsComponent& set_position(const Hex& pos) {
					_storage.set_position(_id, pos);
					return *this;
				}

				const Hex& position() const {
					return _storage.position(_id);
				}

			private:
				PhysicsStorage& _storage;
				id_t _id;
		}; // PhysicsComponent

		PhysicsStorage() {
		}

		void set_position(id_t id, const Hex& pos) {
			auto from = _positions[id];
			_positions[id] = pos;
			printf("set_position id=%ld \n", id);
			_entity_moved.notify({ id, from, _positions[id] });
		}

		const Hex& position(id_t id) const {
			return _positions[id];
		}

		PhysicsComponent operator[](id_t id) {
			return { *this, id };
		}

		PhysicsComponent make_component() {
			_positions.emplace_back();
			auto id = size() - 1;
			return { *this, id };
		}

		size_t size() {
			return _positions.size();
		}

		/**
		 * Observable that notifies whenever an entity changes position.
		 */
		Observable<EntityMoved>& entity_moved() {
			return _entity_moved;
		}

	private:
		std::vector<Hex> _positions;
		Observable<EntityMoved> _entity_moved;
};

using PhysicsComponent = PhysicsStorage::PhysicsComponent;

#endif // _PHYSICS_STORAGE_H_

