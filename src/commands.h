
#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#include <variant>

/**
 * Some abstract commands for controlling user interface, units, etc...
 */

struct BaseCommand {
};

struct DragCommand: BaseCommand {
	DragCommand(int xrel, int yrel) : xrel(xrel), yrel(yrel) {}
	int xrel;
	int yrel;
};

struct ZoomCommand: BaseCommand {
	ZoomCommand(int y): y(y) {}
	int y;
};

struct DebugClickCommand: BaseCommand {
	DebugClickCommand(int x, int y): x(x), y(y) {}
	int x;
	int y;
};

using Command = std::variant<
	DragCommand,
	ZoomCommand,
	DebugClickCommand
>;

#endif // _COMMANDS_H_
