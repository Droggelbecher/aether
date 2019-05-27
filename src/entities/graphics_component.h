
#ifndef _GRAPHICS_COMPONENT_H_
#define _GRAPHICS_COMPONENT_H_

#include <cmath>

#include <fmt/printf.h>
#include <xtensor/xtensor.hpp>

#include "../graphics/texture.h"
#include "../map/hex_coordinate.h"

class GraphicsComponent {

	public:
		GraphicsComponent() {
		}

		GraphicsComponent(const GraphicsComponent& other) {
			*this = other;
		}

		GraphicsComponent& operator=(const GraphicsComponent& other) {
			_texture = other._texture;
			_hovering_t = other._hovering_t;
			_hovering_a = other._hovering_a;
			_hovering_p = other._hovering_p;
			return *this;
		}

		Texture& texture() { return _texture; }

		void update(std::chrono::nanoseconds dt) {
			_hovering_t += dt;
			if(_hovering_t / 1000000000L >= _hovering_p) {
				_hovering_t -= _hovering_p;
			}
		}

		void set_hovering(double amplitude, std::chrono::nanoseconds period, std::chrono::nanoseconds b = std::chrono::nanoseconds{ 0 } ) {
			_hovering_t = -b;
			_hovering_a = amplitude;
			_hovering_p = period;
		}

		Hex transform(Hex p) {
			auto offset = _hovering_a * sin(2. * M_PI * (static_cast<double>(_hovering_t.count()) / _hovering_p.count()));
			//fmt::print("offs={}\n", offset);
			return { p.q(), p.r(), p.z() + offset };
		}

		//static
		//xt::xtensor<double, 2> transform_batch(xt::xtensor<double, 2> positions) {
			//return positions + xt::xtensor<double, 2>{ { 0., 0., offset } };
		//}

	private:
		Texture _texture;
		std::chrono::nanoseconds _hovering_t { 0 };
		double _hovering_a = 0.;
		std::chrono::nanoseconds _hovering_p { 1000000000L };
};

#endif // _GRAPHICS_COMPONENT_H_

