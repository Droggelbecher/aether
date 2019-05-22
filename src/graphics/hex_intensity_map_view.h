
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

// TODO: rather think of a general map / playing field view that shows different things
class HexIntensityMapView {

public:
	HexIntensityMapView(Screen& screen, Coordi size)
		: _texture(screen, size), _scale(100.0) {
	}

	void render() {
		update_texture();
		_texture.render_to_screen();
	}

	void move(double xrel, double yrel) {
		_offset += xt::xarray<double>{ xrel, yrel };
	}

private:
	void update_texture() {
		using xt::xarray;

		using xt::view;
		using xt::all;
		using xt::range;
		using xt::strided_view;
		//using xt::ellipsis;

		//double sz = 100;
		auto r = _texture.render_start();

		auto hex = transform_3d_2d(hex_polygon());

		double h = sqrt(3.) / 2.;
		auto offsets = xarray<double>{
			{ 0. , 0., 0. },
			{ 1.5, h,  0. },
			{ 3. , 0., 0. },
			//{ 1.5, .5 }
		};
		//offsets += xarray<double>{ 100, 100, 0. };

		for(size_t i = 0; i < offsets.shape()[0]; i++) {
			auto offset = transform_3d_2d(view(offsets, i, all()));

			auto poly = transform_2d_screen(hex + offset);
			auto xs = xarray<int16_t>(view(poly, all(), 0));
			auto ys = xarray<int16_t>(view(poly, all(), 1));

			filledPolygonRGBA(r, xs.data(), ys.data(), xs.size(), 100, 100, 100, 255);
			aapolygonRGBA(r, xs.data(), ys.data(), xs.size(), 255, 255, 255, 255);
		}
	}

	xt::xarray<double> hex_polygon() {
		// edge length = 1, center at 0/0
		// "flat" rotation
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

	xt::xarray<double> transform_3d_2d(xt::xarray<double> a) {
		using xt::xarray;

		// http://www.petercollingridge.co.uk/tutorials/svg/isometric-projection/
		static const double phi = 0.25 * M_PI; 
		static const xarray<double> rotate_z = {
			{ cos(phi), -sin(phi), 0. },
			{ sin(phi), cos(phi), 0. },
			{ 0., 0., 1. },
		};

		static const double alpha = atan(sqrt(2.));
		static const xarray<double> rotate_x = {
			{ 1., 0., 0. },
			{ 0., cos(alpha), -sin(alpha) },
			{ 0., sin(alpha), cos(alpha) }
		};

		static const auto M = xt::linalg::dot(rotate_x, rotate_z);
		return xt::transpose(view(
			xt::linalg::dot(
				M,
				xt::transpose(a)
			),
			xt::range(0, 2),
			xt::all()
		));
	}

	xt::xarray<double> transform_2d_screen(xt::xarray<double> a) {
		return a * _scale + _offset;
	}

	//HexIntensityMap _intensity_map;
	Texture _texture;
	double _scale;
	xt::xarray<double> _offset = { 100., 100. };
};


#endif // _HEX_INTENSITY_MAP_VIEW_H_
