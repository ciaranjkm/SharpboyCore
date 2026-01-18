#include <Utilities/SBDebug.h>

SBDebug::SBDebug(ComponentManager* component_manager) {
	//assign pointers to objects from component manager
	cpu = component_manager->get_cpu();
	if (cpu != nullptr) {
		cpu_available = true;
	}


	//check if ptrs are valid and allow debug if they are -> limited if not
	if (cpu_available && true) {
		debug_available = true;
	}
	else {
		debug_available = false;
	}
}

SBDebug::~SBDebug() {
	cpu = nullptr;
}

s_registers* SBDebug::get_cpu_registers() {
	return cpu->get_registers();
}

bool SBDebug::get_flag_state(e_flags flag) {
	u8 f = cpu->get_registers()->f;

	if (flag >= fHCARRY && flag <= fZERO) {
		return f & (1 << flag);
	}

	return false;
}