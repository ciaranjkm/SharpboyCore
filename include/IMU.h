#pragma once

#include "Utilities/Common.h"
#include <array>

/*
	Internal Memory Unit,
		holds hram, wram, echo and io that needs somewhere
*/

const int WRAM_SIZE = 0x2000;
const int HRAM_SIZE = 0x7f;

struct s_imu_io {
	u8 IE = 0x00;
};

class IMU {
public:
	void reset();

	u8 read(u16 address);
	void write(u16 address, u8 value);

private:
	std::array<u8, WRAM_SIZE> m_wram = std::array<u8, WRAM_SIZE>();
	std::array<u8, HRAM_SIZE> m_hram = std::array<u8, HRAM_SIZE>();

	s_imu_io m_io = {};

private:
	u8 read_io(u16 address);
	void write_io(u16 address, u8 value);
};