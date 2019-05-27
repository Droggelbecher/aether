
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

	using xt2d = xt::xtensor_fixed<double, xt::xshape<2> >;
	using xt3d = xt::xtensor_fixed<double, xt::xshape<3> >;
	
	xt::xarray<double> hex_polygon() {
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
	xt::xarray<double> transform_hex_3d(Hex hex) {
		return {
			1.5 * hex.q(),
			sqrt(3.) / 2. * hex.q() + sqrt(3.) * hex.r(),
			hex.z()
		};
	}

	/**
	 * Transform a 3d coordinate to a 2d axonometric projected coordinate
	 */
	xt::xarray<double> transform_3d_2d(xt::xarray<double> a) {
		/*
		using xt::xarray;

		// http://www.petercollingridge.co.uk/tutorials/svg/isometric-projection/
		static const double phi = 0.0 * M_PI; 
		static const xarray<double> rotate_z = {
			{ cos(phi), -sin(phi), 0. },
			{ sin(phi), cos(phi), 0. },
			{ 0., 0., 1. },
		};

		static const double alpha = 0.25 * M_PI; //atan(sqrt(2.));
		static const xarray<double> rotate_x = {
			{ 1., 0., 0. },
			{ 0., cos(alpha), -sin(alpha) },
			{ 0., sin(alpha), cos(alpha) }
		};
		static const auto M = xt::linalg::dot(rotate_x, rotate_z);
		return xt::transpose(view(
					xt::linalg::dot(M, xt::transpose(a)),
					xt::range(0, 2),
					xt::all()
		));
		*/

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


		//return {
			//a[0] * calpha - a[1] * salpha,
			//a[0] * sacb + a[1] * cacb - a[2] * sbeta
		//};
	}


} // namespace

class MapView: public UIElement {

	public:

		MapView(SDL_Renderer* renderer, Coordi size, MapLayer<bool>& walkable, EntityStorage& storage)
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

		void render(SDL_Renderer *renderer) override{
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

			auto hex = transform_3d_2d(hex_polygon());

			for(Hex p: _walkable) {
				auto w = _walkable[p];

				if(!w) { continue; }

				auto offset = transform_3d_2d(transform_hex_3d(p));
				auto poly = transform_2d_screen(hex + offset);
				auto xs = xarray<int16_t>(view(poly, all(), 0));
				auto ys = xarray<int16_t>(view(poly, all(), 1));

				filledPolygonRGBA(renderer, xs.data(), ys.data(), xs.size(), 200, 200, 200, 80);
				aapolygonRGBA(renderer, xs.data(), ys.data(), xs.size(), 255, 255, 255, 120);
			}

			//_render_coordinates(renderer);
			_dirty = false;
		}

		//void _render_coordinates(SDL_Renderer* renderer) {
			//gfxPrimitivesSetFont(gfxPrimitivesFontdata, 8, 8);
			//for(Hex p: _walkable) {
				//std::string s = fmt::format("{}", p);
				//auto offset = transform_3d_2d(transform_hex_3d(p)) - Coordi{(int)s.size()*4, 4};
				//stringRGBA(renderer, offset[0], offset[1], s.c_str(), 0xff, 0xff, 0xff, 0xff);
				////auto w = _walkable[p];
			//}
		//}

		void _render_entities(SDL_Renderer* renderer) {
			auto guard = _canvas.begin_render_onto(renderer);

			//auto visual_positions = _storage.graphics().transform(_storage.positions());
			//auto screen_positions =
				//transform_2d_screen(
				//transform_3d_2d(
				//transform_hex_3d(
					//visual_positions
				//)));


			for(auto e: _storage) {
				auto p = transform_2d_screen(
						transform_3d_2d(
						transform_hex_3d(
							e.graphics().transform(
								e.physics().position()
							)
						)));
				
				auto& tex = e.graphics().texture();
				auto sz = tex.size() * _scale / _scale_max;
				p -= sz / 2.;
				tex.render(renderer, p, sz);
			}
		}

		/**
		 * Transformations due to panning & zooming
		 */
		xt::xarray<double> transform_2d_screen(xt::xarray<double> a) {
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

