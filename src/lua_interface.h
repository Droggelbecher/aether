
#ifndef _LUA_INTERFACE_H_
#define _LUA_INTERFACE_H_

#include <sol/sol.hpp>
#include <string_view>
#include <iostream>

#include "entities/entity_storage.h"

namespace {
	sol::state lua;
}

void init_lua(
		EntityStorage& storage,
		GameLoop& game_loop
) {

	lua.open_libraries();
	lua["storage"] = &storage;
	lua["quit"] = [&]() { game_loop.stop(); };
}

std::string run_expression(std::string s) {
	//auto r = lua.script("return " + s).get<std::string>();
	auto r = lua.script(s).get<std::string>();
	return r;
	//std::cout << "FROM LUA: {{" << r << "}}" << std::endl;
}


#endif // _LUA_INTERFACE_H_

