
#ifndef _GRAPHICS_STORAGE_H_
#define _GRAPHICS_STORAGE_H_

#include <cmath>
#include <chrono>
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

				GraphicsComponent& operator=(const GraphicsComponent& other) = delete;

				GraphicsComponent& copy_from(const GraphicsComponent& other, SDL_Renderer *sdl_renderer) {
					set_texture(other.texture().clone(sdl_renderer));
					set_hovering(other.hovering());
					set_screen_position(other.screen_position());
					return *this;
				}

				GraphicsComponent& set_hovering(double amplitude, std::chrono::nanoseconds period, std::chrono::nanoseconds b = std::chrono::nanoseconds{ 0 } ) {
					auto h = _storage.hovering(_id);
					h.t = -b;
					h.a = amplitude;
					h.p = period;
					_storage.set_hovering(_id, h);
					return *this;
				}

				GraphicsComponent& set_texture(Texture&& texture) {
					_storage.set_texture(_id, std::move(texture));
					return *this;
				};

				const Texture& texture() const {
					return _storage.texture(_id);
				};

				Hex offset() const {
					return { 0, 0, hovering().offset };
				}

				GraphicsComponent& set_screen_position(const Coord2i& pos) {
					_storage.set_screen_position(_id, pos);
					return *this;
				}

				const Coord2i& screen_position() const {
					return _storage.screen_position(_id);
				}

			private:
				void set_hovering(const Hovering& hovering) {
					return _storage.set_hovering(_id, hovering);
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

		void set_hovering(id_t id, const Hovering& hov) {
			_hoverings[id] = hov;
		}

		const Hovering& hovering(id_t id) const {
			return _hoverings[id];
		}

		void set_texture(id_t id, Texture&& tex) {
			_textures[id] = std::move(tex);
		}

		const Texture& texture(id_t id) const {
			return _textures[id];
		}

		void set_screen_position(id_t id, const Coord2i& pos) {
			_screen_position[id] = pos;
		}

		const Coord2i& screen_position(id_t id) {
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

		std::vector<Texture> _textures;
		std::vector<Hovering> _hoverings;
		std::vector<Coordi> _screen_position;

};

using GraphicsComponent = GraphicsStorage::GraphicsComponent;

#endif // _GRAPHICS_STORAGE_H_
