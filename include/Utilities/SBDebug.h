#pragma once

#include "../ComponentManager.h"
#include "../Common.h"

class SBDebug {
public:
	//CONSTRUCTOR + DESTRUCTOR
	SBDebug(ComponentManager* component_manager);
	~SBDebug();

	//PUBLIC FUNCTIONS
	/*   CPU   */
	s_registers* get_cpu_registers();
	bool get_flag_state(e_flags flag);

	/*   PPU   */

	/*   MEMORY   */

	/*   JOYPAD   */

	/*   AUDIO   */

private:
	//COMPONENT REFERENCES
	CPU* cpu = nullptr;

	bool debug_available = false;
	bool cpu_available = false;

};