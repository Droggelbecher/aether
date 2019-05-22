
#ifndef _COMMANDS_H_
#define _COMMANDS_H_

struct Command {
	//virtual void operator()() = 0;
	
	enum Type { DRAG };

	Type type;
	
	union {
		struct {
			double xrel, yrel;
		} drag;
	};
};

Command drag(double xrel, double yrel) {
	Command r { Command::DRAG };
	r.drag = { xrel, yrel };
	return r;
}

#endif // _COMMANDS_H_
