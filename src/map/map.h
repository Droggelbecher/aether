
#ifndef _MAP_H_
#define _MAP_H_

#include <vector>
#include <cassert>
#include <memory>

#include "hex_coordinate.h"
#include "map_layer.h"
#include "../entities/entity_storage.h"

namespace {

	class HexIterator {
		public:
			HexIterator(Hex i, int r_max): _i(i), _r_max(r_max) {
			}

			Hex operator*() { return _i; }
			HexIterator& operator++() {
				if(_i.r() + 1 >= _r_max) {
					_i = { _i.q() + 1, 0 };
				}
				else {
					_i = { _i.q(), _i.r() + 1 };
				}
				return *this;
			}
			bool operator==(const HexIterator& other) const {
				return _i == other._i;
			}
			bool operator!=(const HexIterator& other) const {
				return !(*this == other);
			}
		private:
			Hex _i;
			int _r_max;
	};

} // namespace


class Map {
	public:
		using id_t = std::size_t;
		static constexpr id_t no_id = -1;

		Map(Hex dimensions, EntityStorage& storage)
			: _dimensions(dimensions),
			_walkable(dimensions),
			_entity(dimensions, no_id),
			_storage(storage)
		{
			_storage.physics_storage().entity_moved().observe(
					[this](const auto& evt) { move_entity(evt.from, evt.to, evt.id); }
			);

			_storage.entity_created().observe(
					[this](const auto& evt) { put_entity(_storage[evt.id].physics().position(), evt.id); }
			);
		}

		auto begin() {
			return HexIterator({0, 0}, _dimensions.r());
		}

		auto end() {
			return HexIterator({_dimensions.q(), 0}, _dimensions.r());
		}

		void put_entity(const Hex& pos, id_t id) {
			auto i_pos = hex2index(pos);
			printf("put_entity i_pos=%lu\n", i_pos);
			_entity[i_pos] = id;
		}

		void move_entity(const Hex& from, const Hex& to, id_t id) {
			auto i_from = hex2index(from);
			auto i_to = hex2index(to);
			//assert(_entity[i_from] != no_id);
			//assert(_entity[i_to] == no_id);

			printf("move_entity id=%ld i_from=%lu i_to=%lu\n", id, i_from, i_to);
			_entity[i_to] = id; //_entity[i_from];
			_entity[i_from] = no_id;
		}

		EntityStorage::Entity entity(const Hex& pos) {
			auto i_pos = hex2index(pos);
			return _storage[_entity[i_pos]];
		}

		bool walkable(const Hex& pos) {
			auto i_pos = hex2index(pos);
			return _walkable[i_pos];
		}

		void set_walkable(const Hex& pos, bool value=true) {
			auto i_pos = hex2index(pos);
			_walkable[i_pos] = value;
		}


	private:

		size_t size() { return _dimensions.q() * _dimensions.r(); }

		size_t hex2index(Hex h) {
			assert(h.q() < _dimensions.q());
			assert(h.q() >= 0);
			assert(h.r() < _dimensions.r());
			assert(h.r() >= 0);
			return h.q() * _dimensions.r() + h.r();
		}

		Hex index2hex(size_t i) {
			int i_ = i;
			return { i_ % _dimensions.q(), static_cast<int>(i_ / _dimensions.q()) };
		}

		Hex _dimensions;

		MapLayer<bool> _walkable;
		MapLayer<id_t> _entity;
		EntityStorage& _storage;
};


//std::unique_ptr<Map> make_hexagon(size_t n) {
	//auto map = std::make_unique<Map>(2 * n - 1);
	//return map;
//}

#endif // _MAP_H_

