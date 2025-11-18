#pragma once

#include "../Utilities/Common.h"
#include "../Utilities/IO.h"
#include "Joypad.h"

#include <vector>

/*
	Internal Memory Unit,
		holds hram, wram, echo and io that needs somewhere to live ;(
*/

const int WRAM_SIZE = 0x2000;
const int HRAM_SIZE = 0x7f;

struct s_imu_io {
	u8 sb = 0x00;
	u8 sc = 0x00;
	u8 joyp = 0x00;
};

class IMU {
public:
	IMU();

	//RESET 
	void reset(bool using_boot_rom = false);

	//MEMORY ACCESS
	u8 read(u16 address);
	void write(u16 address, u8 value);

	u8 read_io(u16 address);
	void write_io(u16 address, u8 value);

	//UPDATE JOYPAD STATE NOT USED IN CLI
	void update_joypad(s_joypad_state new_joypad_state);

private:
	std::vector<u8> m_wram = std::vector<u8>();
	std::vector<u8> m_hram = std::vector<u8>();

	s_imu_io m_io = {};
	s_joypad_state m_joypad_state = {};
};