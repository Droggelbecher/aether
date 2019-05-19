
#ifndef _INTENSITY_MAP_PERLIN_H_
#define _INTENSITY_MAP_PERLIN_H_

#include <random>
#include <algorithm>
#include <iterator>
#include <tuple>
#include <functional>
#include <array>
#include <cmath>

#include <fmt/format.h>

#include "intensity_map.h"
#include "coordinate.h"

IntensityMap generate_white_noise(size_t sx, size_t sy) {
	IntensityMap im(sx, sy);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> uniform;

	for(size_t ix = 0; ix < sx; ix++) {
		for(size_t iy = 0; iy < sy; iy++) {
			im(ix, iy) = uniform(gen);
			fmt::print("{0} {1} := {2}\n", ix, iy, im(ix, iy));
		}
	}
	return im;
}

IntensityMap generate_subdivision(Coordi sz, double roughness=1.5, uint32_t seed=0) {
	IntensityMap im(sz);

	std::mt19937 gen(seed);
	std::uniform_real_distribution<> uniform(-1., 1.);

	auto avg  = [](auto a, auto b) { return (a + b) / 2; };
	auto avg4 = [](auto a, auto b, auto c, auto d) { return (a + b + c + d) / 4; };

	std::function<void(Coordi, Coordi, double)> subdivide;
	subdivide = [&](Coordi tl, Coordi br, double f) -> void {
		const Coordi bl = {tl.x, br.y};
		const Coordi tr = {br.x, tl.y};
		const Coordi c  = {avg(tl.x, br.x), avg(tl.y, br.y)};
		const Coordi l  = {tl.x, c.y};
		const Coordi r  = {br.x, c.y};
		const Coordi b  = {c.x, br.y};
		const Coordi t  = {c.x, tl.y};

		im(r) = avg(im(tr), im(br)) + f * uniform(gen);
		im(b) = avg(im(bl), im(br)) + f * uniform(gen);
		im(c) = avg4(im(bl), im(tr), im(br), im(tl)) + f * uniform(gen);

		if(abs(l.x - r.x) >= 2 && abs(t.y - b.y) >= 2) {
			subdivide(tl, c, f / roughness);
			subdivide(t, r, f / roughness);
			subdivide(l, b, f / roughness);
			subdivide(c, br, f / roughness);
		}
	};

	const Coordi tl = {0, 0};
	const Coordi br = {sz.x - 1, sz.y - 1};
	const Coordi bl = {tl.x, br.y};
	const Coordi tr = {br.x, tl.y};
	const Coordi c  = {avg(tl.x, br.x), avg(tl.y, br.y)};
	const Coordi l  = {tl.x, c.y};
	const Coordi t  = {c.x, tl.y};
	im(tl) = uniform(gen);
	im(br) = uniform(gen);
	im(bl) = uniform(gen);
	im(tr) = uniform(gen);
	im(l) = avg(im(tl), im(bl)) + (1. / roughness) * uniform(gen);
	im(t) = avg(im(tl), im(tr)) + (1. / roughness) * uniform(gen);

	subdivide(tl, br, 1.);
	im.normalize();

	return im;
}

#endif // _INTENSITY_MAP_PERLIN_H_

