
#ifndef _MAP_VIEW_H_
#define _MAP_VIEW_H_

#include <SDL2/SDL2_gfxPrimitives.h>
//#include <SDL2/SDL2_gfxPrimitives_font.h>
#include <fmt/printf.h>
#include <xtensor/xfixed.hpp>
#include <xtensor/xshape.hpp>
#include <xtensor/xarray.hpp>
#include <xtensor/xview.hpp>
#include <xtensor-blas/xlinalg.hpp>

#include "../ui_element.h"
#include "../commands.h"
#include "../entities/entity_storage.h"
#include "../map/map_layer.h"

namespace {

	CoordBatch3d hex_polygon() {
		static const double h = sqrt(3.) / 2.;
		return {
			{ - .5, -h, 0. },
			{   .5, -h, 0. },
			{  1.0,  0, 0. },
			{   .5,  h, 0. },
			{ - .5,  h, 0. },
			{ -1.0,  0, 0. },
		};
	}

	/**
	 * Transform hex-coordinate (with z-value) to 3d cartesian coordinate
	 */
	//xt::xarray<double> transform_hex_3d_batch(const Hex& hex) {
		//return {
			//1.5 * hex.q(),
			//sqrt(3.) / 2. * hex.q() + sqrt(3.) * hex.r(),
			//hex.z()
		//};
	//}

	Coord3d transform_hex_3d(const Hex& hex) {
		return {
			1.5 * hex.q(),
			sqrt(3.) / 2. * hex.q() + sqrt(3.) * hex.r(),
			hex.z()
		};
	}

	/**
	 * Transform a 3d coordinate to a 2d axonometric projected coordinate
	 */
	CoordBatch2d transform_3d_2d_batch(xt::xtensor<double, 2> a) {
		static const double alpha = 0. * M_PI;
		static const double beta = .25 * M_PI;

		static const double calpha = cos(alpha);
		static const double salpha = sin(alpha);
		static const double cbeta = cos(beta);
		static const double sbeta = sin(beta);
		static const double sacb = salpha * cbeta;
		static const double cacb = calpha * cbeta;

		static const xt::xtensor_fixed<double, xt::xshape<3, 3> > M = {
			{ calpha, salpha, 0. },
			{ sacb, cacb, -sbeta },
			{ 0, 0, 0 }
		};

		return xt::transpose(view(
					xt::linalg::dot(M, xt::transpose(a)),
					xt::range(0, 2),
					xt::all()
		));
	}

	Coord2d transform_3d_2d(Coord3d a) {
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

		return {
			a[0] * calpha - a[1] * salpha,
			a[0] * sacb + a[1] * cacb - a[2] * sbeta
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
			_offset += xt::xarray<double>{
				static_cast<double>(cmd.xrel),
				static_cast<double>(cmd.yrel)
			};
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
			using xt::xarray;
			using xt::view;
			using xt::all;
			using xt::range;
			using xt::strided_view;

			if(!_dirty) {
				return;
			}

			// guard will just make sure to reset renderer state when it goes out of scope
			auto guard = _grid_canvas.begin_render_onto(renderer, Texture::CLEAR);

			auto hex = transform_3d_2d_batch(hex_polygon());

			for(Hex p: _walkable) {
				auto w = _walkable[p];

				if(!w) { continue; }

				auto offset = transform_3d_2d(transform_hex_3d(p));
				auto poly = transform_2d_screen_batch(hex + offset);
				auto xs = xarray<int16_t>(view(poly, all(), 0));
				auto ys = xarray<int16_t>(view(poly, all(), 1));

				filledPolygonRGBA(renderer, xs.data(), ys.data(), xs.size(), 200, 200, 200, 80);
				aapolygonRGBA(renderer, xs.data(), ys.data(), xs.size(), 255, 255, 255, 120);
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
		CoordBatch2i transform_2d_screen_batch(CoordBatch2d a) {
			return a * _scale + _offset;
		}

		Coord2i transform_2d_screen(Coord2d a) {
			return a * _scale + _offset;
		}

		Texture _canvas;
		Texture _grid_canvas;
		static constexpr double _scale_min = 10.;
		static constexpr double _scale_max = 100.;
		double _scale = _scale_max;
		xt::xarray<double> _offset = { 100., 100. };
		MapLayer<bool>& _walkable;
		EntityStorage& _storage;
		bool _dirty = true;
};

#endif // _MAP_VIEW_H_

