
#ifndef _INTENSITY_MAP_H_
#define _INTENSITY_MAP_H_

#include <cassert>
#include <random>
#include <memory>
#include <cstdint>
#include <iterator>
#include <algorithm>
#include <vector>

#include "coordinate.h"

class IntensityMap {
	public:
		using Intensity = float;

		IntensityMap(Coordi sz)
			: _size(sz), _intensity(area()) {
		}
		
		IntensityMap(size_t sx, size_t sy) : _size(sx, sy), _intensity(area()) {
		}

		size_t area() const { return _size.x * _size.y; }
		Coordi size() const { return _size; }

		Intensity& operator()(int ix, int iy) {
			return operator()({ ix, iy });
		}

		Intensity& operator()(Coordi c) {
			if(c.x >= _size.x || c.y >= _size.y) {
				throw std::runtime_error("index out of bounds");
			}
			return _intensity[c.y * _size.x + c.x];
		}

		Intensity operator()(int ix, int iy) const {
			return operator()({ ix, iy });
		}

		Intensity operator()(Coordi c) const {
			if(c.x >= _size.x || c.y >= _size.y) {
				return _default;
			}
			return _intensity[c.y * _size.x + c.x];
		}

		void scale(Intensity f) {
			std::transform(
				_intensity.begin(), _intensity.end(),
				_intensity.begin(),
				[=](auto x) { return x * f; }
			);
		}

		void normalize() {
			Intensity max = *std::max_element(std::begin(_intensity), std::end(_intensity));
			Intensity min = *std::min_element(std::begin(_intensity), std::end(_intensity));

			if(max > min) {
				std::transform(
					_intensity.begin(), _intensity.end(),
					_intensity.begin(),
					[=](auto x) { return (x - min) / (max - min); }
				);
			}
		}

		void operator+=(const IntensityMap& other) {
			assert(other.size() == size());
			for(size_t i = 0; i <= area(); i++) {
				_intensity[i] += other._intensity[i];
			}
		}

	private:
		Coordi _size;
		Intensity _default = 0.5;
		std::vector<Intensity> _intensity;
};


#endif // _INTENSITY_MAP_H_

