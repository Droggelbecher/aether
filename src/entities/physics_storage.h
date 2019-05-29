
#ifndef _PHYSICS_STORAGE_H_
#define _PHYSICS_STORAGE_H_

#include <cstdint>
#include <vector>
#include "../map/hex_coordinate.h"

class PhysicsStorage {

	public:
		using id_t = std::size_t;

		struct PhysicsComponent {
			public:
				PhysicsComponent(PhysicsStorage& storage, id_t id)
					: _storage(storage), _id(id) {
				}

				PhysicsComponent(const PhysicsComponent& other)
					: _storage(other._storage), _id(other._id) {
				}

				PhysicsComponent& operator=(const PhysicsComponent& other) {
					position() = other.position();
					return *this;
				}

				Hex& position() {
					return _storage.position(_id);
				}

				const Hex& position() const {
					return _storage.position(_id);
				}

			private:
				PhysicsStorage& _storage;
				id_t _id;
		}; // PhysicsComponent

		Hex& position(id_t id) {
			return _positions[id];
		}

		PhysicsComponent operator[](id_t id) {
			return { *this, id };
		}

		PhysicsComponent make_component() {
			_positions.emplace_back();
			return { *this, size() - 1 };
		}

		size_t size() {
			return _positions.size();
		}

	private:
		std::vector<Hex> _positions;
};

using PhysicsComponent = PhysicsStorage::PhysicsComponent;

#endif // _PHYSICS_STORAGE_H_

