
#ifndef _HEX_INTENSITY_MAP_VIEW_H_
#define _HEX_INTENSITY_MAP_VIEW_H_

#include <cstdint>

#include <SDL2_gfxPrimitives.h>

#include <xtensor/xarray.hpp>
#include <xtensor/xview.hpp>
#include <xtensor/xio.hpp>
#include <xtensor/xstridedview.hpp>

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

		auto r = _texture.render_start();

		aacircleRGBA(r, 50, 50, 20, 255, 0, 0, 255);

		auto hex = hex_polygon() * 20;

		auto offsets = xarray<double>{
			{ 50, 50 },
			{ 70, 50 },
			{ 50, 70 }
		};

		for(auto it = offsets.begin({2}); it != offsets.end({2}); ++it) {
			auto offset = *it;
			fmt::print("offset={0} {1}\n", offset(0), offset(1));
		
			auto poly = hex + offset; //xarray<double>{ 50, 50 };
			auto xs = xarray<int16_t>(view(poly, all(), 0));
			auto ys = xarray<int16_t>(view(poly, all(), 1));
			aapolygonRGBA(r, xs.data().data(), ys.data().data(), xs.size(), 0, 255, 0, 255);
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
			{ - .5, -h },
			{   .5, -h },
			{  1.0,  0 },
			{   .5,  h },
			{ - .5,  h },
			{ -1.0,  0 },
		};
	}


	//HexIntensityMap _intensity_map;
	Texture _texture;
	double _scale;
};


#endif // _HEX_INTENSITY_MAP_VIEW_H_
