
#ifndef _INTENSITY_MAP_PERLIN_H_
#define _INTENSITY_MAP_PERLIN_H_

#include <random>
#include <algorithm>
#include <iterator>
#include <tuple>
#include <array>

#include "intensity_map.h"

namespace {
	float affine(float a, float b, float w) {
		return w * b + (1.0 - w) * a;
	}

	//float dot(int ix, int 
}

void generate_white_noise(IntensityMap& im) {
	int sx, sy;
	std::tie(sx, sy) = im.size();

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> uniform(255);

	for(int ix = 0; ix < sx; ix++) {
		for(int iy = 0; iy < sy; iy++) {
			im(ix, iy) = uniform(gen);
		}
	}
}

void generate_perlin(IntensityMap& im) {

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> uniform;

	// Generate Gradients 
	// y component implicitly by 1 = x^2 + y^2 <=> y = sqrt(1 - x^2)
	std::vector<float> gradient_x;
	gradient_x.resize(im.area());
	std::generate(std::begin(gradient_x), std::end(gradient_x), [&]{ return uniform(gen); });

	//std::vector<float> 
	// distance vectors
	
	// given a grid point at (x, y), compute the dis
	//dx = x
	
}

#endif // _INTENSITY_MAP_PERLIN_H_

