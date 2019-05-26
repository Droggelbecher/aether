
#ifndef _ACCUMULATOR_H_
#define _ACCUMULATOR_H_

#include <cstdint>

/**
 * Handy helper for accumulating values (time spend, bytes read) in a loop
 * and triggering an action when a certain threshold has been summed up.
 * Provides access to avg. value per call (useful for eg. FPS) as well as total
 * sum of values (useful for eg. file reading).
 */
template<typename T>
class Accumulator {
	public:
		Accumulator(T threshold): _threshold(threshold) {
		}

		/**
		 * Accumulate value `v`. Return true iff the threshold value or more
		 * has been accumulated this way. In that case reset the threshold counter.
		 */
		bool operator()(T v) {
			if(_reset) {
				_value = T();
				_calls = 0;
				_reset = false;
			}

			_value += v;
			_total += v;
			_calls++;

			if(_value >= _threshold) {
				_reset = true;
			}
			return _reset;
		}

		auto mean() {
			return _value / _calls;
		}

		auto total() {
			return _total;
		}

		auto calls() {
			return _calls;
		}

		auto value() {
			return _value;
		}

	private:
		T _value = T();
		T _threshold = T();
		T _total = T();
		std::size_t _calls = 0;
		bool _reset = false;
};

#endif // _ACCUMULATOR_H_

