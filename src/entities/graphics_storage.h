
#ifndef _GRAPHICS_STORAGE_H_
#define _GRAPHICS_STORAGE_H_

#include <cmath>
#include <fmt/printf.h>
#include "../graphics/texture.h"
#include "../map/hex_coordinate.h"

class GraphicsStorage {

		struct Hovering {
			std::chrono::nanoseconds t;
			std::chrono::nanoseconds p;
			double a;
			double offset;
		};

	public:
		using id_t = std::size_t;

		struct GraphicsComponent {
			public:
				GraphicsComponent(GraphicsStorage& storage, id_t id)
					: _storage(storage), _id(id) {
				}

				GraphicsComponent(const GraphicsComponent& other)
					: _storage(other._storage), _id(other._id) {
				}

				GraphicsComponent& operator=(const GraphicsComponent& other) {
					texture() = other.texture();
					hovering() = other.hovering();
					screen_position() = other.screen_position();
					return *this;
				}

				//void update_screen_position(const Hex& p) {
					//screen_position() = {
						//p.q(), p.r(), p.z() + hovering().offset
					//};
				//}

				void set_hovering(double amplitude, std::chrono::nanoseconds period, std::chrono::nanoseconds b = std::chrono::nanoseconds{ 0 } ) {
					auto& h = _storage.hovering(_id);
					h.t = -b;
					h.a = amplitude;
					h.p = period;
				}

				Texture& texture() {
					return _storage.texture(_id);
				};

				const Texture& texture() const {
					return _storage.texture(_id);
				};

				Hex offset() const {
					return { 0, 0, hovering().offset };
				}

				Coordi& screen_position() {
					return _storage.screen_position(_id);
				}

				const Coordi& screen_position() const {
					return _storage.screen_position(_id);
				}

			private:
				Hovering& hovering() {
					return _storage.hovering(_id);
				}

				const Hovering& hovering() const {
					return _storage.hovering(_id);
				}
		
				GraphicsStorage& _storage;
				id_t _id;
		};

		void update(std::chrono::nanoseconds dt) {
			_update_hovering(dt);

		}

		Hovering& hovering(id_t id) {
			return _hoverings[id];
		}

		Texture& texture(id_t id) {
			return _textures[id];
		}

		Coordi& screen_position(id_t id) {
			return _screen_position[id];
		}

		GraphicsComponent make_component() {
			_textures.emplace_back();
			_hoverings.emplace_back();
			_screen_position.emplace_back();
			return { *this, size() - 1 };
		}

		GraphicsComponent operator[](id_t id) {
			return { *this, id };
		}

		size_t size() {
			return _textures.size();
		}

	private:

		void _update_hovering(std::chrono::nanoseconds dt) {
			for(auto& h: _hoverings) {
				h.t += dt;
				h.t %= h.p;
				h.offset = h.a * sin(2. * M_PI * (static_cast<double>(h.t.count()) / h.p.count()));
			}
		}

		//void _update_drawing_position() {
			//for(size_t i = 0; i < size(); i++) {
				//auto& draw = _drawing_positions[i];
				//auto& phys = _s

			//for(auto& p: _drawing_positions) {

		std::vector<Texture> _textures;
		std::vector<Hovering> _hoverings;
		std::vector<Coordi> _screen_position;

};

using GraphicsComponent = GraphicsStorage::GraphicsComponent;

#endif // _GRAPHICS_STORAGE_H_
