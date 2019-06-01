
#ifndef _MAP_VIEW_H_
#define _MAP_VIEW_H_

#include <algorithm>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <fmt/printf.h>

#include "../ui_element.h"
#include "../commands.h"
#include "../entities/entity_storage.h"
#include "../map/map_layer.h"

namespace {

	// Private, not instance-specific functions for map view.
	// Mostly coordinate conversion.
	//
	// We consider the following coordinate systems:
	//
	// Name     Coordinate type Description
	// -------- --------------- -----------------------------------------
	// "hex"    Hex             3d position using hex map-coordinates incl. height values
	// "3d"     Coord3d         3d position with Hex translated to cartesian
	// "2d"     Coord2d         2d position after fixed perspective transformation
	// "screen" Coord2i         2d (int) position after perspective transform AND map view transform (panning+scale)
	//                          (actual pixel coordinates)
	//                          only system that is map-view state dependent

	const double h = sqrt(3.) / 2.;
	std::array<Coord3d, 6> hex_polygon = {
		Coord3d { - .5,  -h, 0. },
		Coord3d {   .5,  -h, 0. },
		Coord3d {  1.0,  0., 0. },
		Coord3d {   .5,   h, 0. },
		Coord3d { - .5,   h, 0. },
		Coord3d { -1.0,  0., 0. },
	};

	/**
	 * Transform hex-coordinate (with z-value) to 3d cartesian coordinate
	 */
	Coord3d transform_hex_3d(const Hex& hex) {
		return {
			1.5 * hex.q(),
			sqrt(3.) / 2. * hex.q() + sqrt(3.) * hex.r(),
			hex.z()
		};
	}

	template<typename iterator_type>
	CoordBatch2d transform_hex_3d(iterator_type begin_, iterator_type end_) {
		CoordBatch2d r;
		std::transform(begin_, end_, std::back_inserter(r), transform_hex_3d);
		return r;
	}

	Hex transform_3d_hex(const Coord3d& c) {
		return {
			static_cast<int>(std::lround((2./3.) * c.x())),
			static_cast<int>(std::lround((1./sqrt(3.)) * c.y() - (1./3.)*c.x())),
			c.z()
		};
	}

	/*
	 * [      1,       0,       0]
	 * [      0,  cos(b), -sin(b)]
	 * [      0,  sin(b),  cos(b)]
	 *
	 * [ cos(a), -sin(a), 0 ]
	 * [ sin(a),  cos(a), 0 ]
	 * [      0,       0, 1 ]
	 *
	 * =>
	 *
	 * [ cos(a), -sin(a), 0 ]
	 * [ sin(a)cos(b), cos(a)cos(b), -sin(b) ]
	 * [ sin(a)sin(b), cos(a)sin(b), cos(b) ]
	 */

	static const double alpha = 0. * M_PI;
	static const double beta = .25 * M_PI;
	static const double calpha = cos(alpha);
	static const double salpha = sin(alpha);
	static const double cbeta = cos(beta);
	static const double sbeta = sin(beta);
	static const double sacb = salpha * cbeta;
	static const double cacb = calpha * cbeta;

	/**
	 * Transform a 3d coordinate to a 2d axonometric projected coordinate
	 */
	Coord2d transform_3d_2d(Coord3d a) {
		return {
			a[0] * calpha - a[1] * salpha,
			a[0] * sacb + a[1] * cacb - a[2] * sbeta
		};
	}

	template<typename iterator_type>
	CoordBatch2d transform_3d_2d(iterator_type begin_, iterator_type end_) {
		CoordBatch2d r;
		std::transform(begin_, end_, std::back_inserter(r),
				[&](const Coord3d& c) { return transform_3d_2d(c); }
		);
		return r;
	}

	Coord3d transform_2d_3d(Coord2d a) {
		return {
			a[0] * (1. - salpha*salpha) + a[1] * calpha * salpha / cbeta,
			-a[0] * salpha + a[1] * calpha / cbeta
		};
	}

} // namespace

class MapView: public UIElement {

	public:

		MapView(SDL_Renderer* renderer, Coord2i size, MapLayer<bool>& walkable, EntityStorage& storage)
			: _canvas(renderer, size),
			  _grid_canvas(renderer, size),
			  _walkable(walkable),
			  _storage(storage)
		{ }

		bool process_command(const Command& command) override {
			return std::visit(
					[&](auto&& cmd) { return process_concrete_command(cmd); },
					command
					);
		}

		bool process_concrete_command(const ZoomCommand& cmd) {
			_scale += cmd.y * 10.;
			_scale = std::min(_scale, _scale_max);
			_scale = std::max(_scale, _scale_min);
			_dirty = true;
			return true;
		}

		bool process_concrete_command(const DragCommand& cmd) {
			_offset += Coord2d {
				static_cast<double>(cmd.xrel),
				static_cast<double>(cmd.yrel)
			};
			_dirty = true;
			return true;
		}

		bool process_concrete_command(const DebugClickCommand& cmd) {
			auto hex = transform_3d_hex(transform_2d_3d(transform_screen_2d({cmd.x, cmd.y})));
			fmt::print("({} {}) -> {}\n", cmd.x, cmd.y, hex);
			_selected = hex;
			_dirty = true;
			return true;
		}

		void update_screen_positions() {
			for(auto e: _storage) {
				const Hex& pos = e.physics().position();
				e.graphics().screen_position() =
					transform_2d_screen(
					transform_3d_2d(
					transform_hex_3d(
						pos + e.graphics().offset()
					)));
			}
		}

		void render(SDL_Renderer *renderer) override {
			_render_grid(renderer);

			_canvas.begin_render_onto(renderer, Texture::CLEAR);
			_grid_canvas.render(renderer);
			_render_entities(renderer);
			_canvas.render(renderer);
		};

	private:

		void _render_grid(SDL_Renderer* renderer) {
			if(!_dirty) {
				return;
			}

			// guard will make sure to reset renderer state when it goes out of scope
			auto guard = _grid_canvas.begin_render_onto(renderer, Texture::CLEAR);

			static auto hex2d = transform_3d_2d(hex_polygon.begin(), hex_polygon.end());

			for(Hex p: _walkable) {
				auto w = _walkable[p];
				if(!w) { continue; }

				auto offset = transform_3d_2d(transform_hex_3d(p));

				std::vector<int16_t> xs;
				std::transform(hex2d.begin(), hex2d.end(), std::back_inserter(xs),
						[&](Coord2d c) { return transform_2d_screen(c + offset).x(); });

				std::vector<int16_t> ys;
				std::transform(hex2d.begin(), hex2d.end(), std::back_inserter(ys),
						[&](Coord2d c) { return transform_2d_screen(c + offset).y(); });

				filledPolygonRGBA(renderer, xs.data(), ys.data(), xs.size(), 200, 200, 200, 80);
				aapolygonRGBA(renderer, xs.data(), ys.data(), xs.size(), 255, 255, 255, 120);
			}

			if(_selected != Hex::invalid) {
				auto offset = transform_3d_2d(transform_hex_3d(_selected));

				std::vector<int16_t> xs;
				std::transform(hex2d.begin(), hex2d.end(), std::back_inserter(xs),
						[&](Coord2d c) { return transform_2d_screen(c + offset).x(); });

				std::vector<int16_t> ys;
				std::transform(hex2d.begin(), hex2d.end(), std::back_inserter(ys),
						[&](Coord2d c) { return transform_2d_screen(c + offset).y(); });

				filledPolygonRGBA(renderer, xs.data(), ys.data(), xs.size(), 50, 200, 50, 200);
				aapolygonRGBA(renderer, xs.data(), ys.data(), xs.size(), 255, 255, 255, 255);
			}

			_dirty = false;
		}

		void _render_entities(SDL_Renderer* renderer) {
			auto guard = _canvas.begin_render_onto(renderer);

			for(auto e: _storage) {
				auto p = e.graphics().screen_position();
				
				auto& tex = e.graphics().texture();
				auto sz = tex.size() * _scale / _scale_max;
				p -= sz / 2.;
				tex.render(renderer, p, sz);
			}
		}

		/**
		 * Transformations due to panning & zooming
		 */
		Coord2i transform_2d_screen(Coord2d a) {
			return (a * _scale + _offset).as_type<int>();
		}

		/**
		 * Transformations due to panning & zooming (batch)
		 */
		CoordBatch2i transform_2d_screen_batch(const CoordBatch2d& a) {
			CoordBatch2i r;
			std::transform(a.begin(), a.end(), std::back_inserter(r),
					[&](const Coord2d& c) { return transform_2d_screen(c); }
			);
			return r;
		}

		Coord2d transform_screen_2d(Coord2i a) {
			return ((a - _offset) / _scale).as_type<Coord2d>();
		}

		Hex _selected = Hex::invalid;

		Texture _canvas;
		Texture _grid_canvas;
		static constexpr double _scale_min = 10.;
		static constexpr double _scale_max = 100.;
		double _scale = _scale_max;
		Coord2d _offset = { 100., 100. };
		MapLayer<bool>& _walkable;
		EntityStorage& _storage;
		bool _dirty = true;
};

#endif // _MAP_VIEW_H_

