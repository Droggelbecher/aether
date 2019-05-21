
#ifndef _HEX_INTENSITY_MAP_VIEW_H_
#define _HEX_INTENSITY_MAP_VIEW_H_

#include <cstdint>
#include <cmath>

#include <SDL2_gfxPrimitives.h>

#include <xtensor/xarray.hpp>
#include <xtensor/xview.hpp>
#include <xtensor/xio.hpp>
#include <xtensor/xstrided_view.hpp>
#include <xtensor-blas/xlinalg.hpp>

#include <fmt/format.h>

//#include "hex_intensity_map.h"
#include "coordinate.h"
#include "texture.h"

class HexIntensityMapView {

public:
	// TODO: Position on screen?
	//HexIntensityMapView(const HexIntensityMap& im, Screen& screen, Coordi size)
		//: _intensity_map(im), _texture(screen, size), _scale(100.0) {
	//}

	HexIntensityMapView(Screen& screen, Coordi size)
		: _texture(screen, size), _scale(100.0) {
	}

	void render() {
		update_texture();
		_texture.render_to_screen();
	}


private:

	void update_texture() {
		using xt::xarray;

		using xt::view;
		using xt::all;
		using xt::range;
		using xt::strided_view;
		//using xt::ellipsis;

		double sz = 100;
		auto r = _texture.render_start();

		auto hex = hex_polygon() * 100;

		double h = sqrt(3.) / 2.;
		auto offsets = xarray<double>{
			{ 0. , 0., 0. },
			{ 1.5, h,  0. },
			{ 3. , 0., 0. },
			//{ 1.5, .5 }
		};
		offsets *= sz;
		offsets += xarray<double>{ 100, 100, 0. };

		for(size_t i = 0; i < offsets.shape()[0]; i++) {
			//auto offset = xt::linalg::dot(view(offsets, i, all()), view_transform());
			//auto offset = xt::linalg::dot(view(offsets, i, all()), view_transform());
			auto offset = project(view(offsets, i, all()));
			fmt::print("offset={0} {1}\n", offset(0), offset(1));
		
			auto poly = project(hex) + offset; //xt::linalg::dot(hex, view_transform()) + offset;
			fmt::print("poly.sh={0} {1}\n", poly.shape()[0], poly.shape()[1]);
			auto xs = xarray<int16_t>(view(poly, all(), 0));
			auto ys = xarray<int16_t>(view(poly, all(), 1));

			aacircleRGBA(r, offset(0), offset(1), sz, 255, 255, 255, 100);

			aapolygonRGBA(r, xs.data(), ys.data(), xs.size(), 220, 230, 240, 255);
		}

		//SDL_SetRenderTarget(r, _sdl_texture);

		//_texture.lock();

		/*
		for(auto hex: _intensity_map.coordinates()) {
			filledPolygonColor(
					_texture.renderer(),
					// TODO: int* xs, int* ys, int n, uint32_t color
			)
		}
		*/

		//_texture.unlock();
	}

	xt::xarray<double> hex_polygon() {
		// edge length = 1, center at 0/0
		// "flat" rotation
		double h = sqrt(3.) / 2.;
		return {
			{ - .5, -h, 0. },
			{   .5, -h, 0. },
			{  1.0,  0, 0. },
			{   .5,  h, 0. },
			{ - .5,  h, 0. },
			{ -1.0,  0, 0. },
		};
	}

	xt::xarray<double> project(xt::xarray<double> a) {
		using xt::xarray;

		// http://www.petercollingridge.co.uk/tutorials/svg/isometric-projection/
		const double phi = 0.25 * M_PI; 
		const xarray<double> rotate_z = {
			{ cos(phi), -sin(phi), 0. },
			{ sin(phi), cos(phi), 0. },
			{ 0., 0., 1. },
		};

		const double alpha = atan(sqrt(2.));
		const xarray<double> rotate_x = {
			{ 1., 0., 0. },
			{ 0., cos(alpha), -sin(alpha) },
			{ 0., sin(alpha), cos(alpha) }
		};

		//const auto M = xt::linalg::dot(rotate_z, rotate_x);
		const auto M = xt::linalg::dot(rotate_x, rotate_z);
		return xt::transpose(view(
			xt::linalg::dot(
				//xt::stack(xt::xtuple(a, xarray<double>{0.})),
				M,
				xt::transpose(a)
			),
			xt::range(0, 2),
			xt::all()
		));
	}

	xt::xarray<double> view_transform() {
		double m = 0.;
		double n = -.3;
		return { {1., m}, {n, 1.} };
	}


	//HexIntensityMap _intensity_map;
	Texture _texture;
	double _scale;
};


#endif // _HEX_INTENSITY_MAP_VIEW_H_
