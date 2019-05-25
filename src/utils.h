
#ifndef _UTIL_H_
#define _UTIL_H_

#include <functional>

struct ScopeGuard {
	~ScopeGuard() { f(); }
	std::function<void()> f;
};

#endif // _UTIL_H_

