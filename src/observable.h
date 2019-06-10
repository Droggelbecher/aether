
#ifndef _OBSERVABLE_H_
#define _OBSERVABLE_H_

#include <functional>

/**
 * Handy class to apply the observer patterns for certain events.
 * In this implementation an observer is any std::function that accepts
 * the user-defined event type as argument.
 * Use as a has-a so that your class can define multiple event types.
 *
 * Use like this:
 *
 * {{{
 *
 * struct MyEvent {
 *   int some_info;
 *   std::string foo;
 * };
 *
 * auto obs = Observable<MyEvent>();
 *
 * // Register observer (must be convertible to std:function<void(const MyEvent&)>
 * obs.observe([] (const MyEvent& evt) { std::cout << event.foo << std::endl; });
 *
 * // Notify all registered observers
 * obs.notify({ 42, "Something happened" });
 *
 * }}}
 */
template<typename Event>
class Observable {
	public:
		using Observer = std::function<void(const Event&)>;

		Observable() { }

		Observable(const Observable& other) = delete;
		Observable& operator=(const Observable& other) = delete;

		void observe(const Observer& observer) {
			_observers.push_back(observer);
		}

		void notify(const Event& event) {
			for(auto& observer: _observers) {
				observer(event);
			}
		}

	private:
		std::vector<Observer> _observers;
};

#endif // _OBSERVABLE_H_

