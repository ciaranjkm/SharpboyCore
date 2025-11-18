#pragma once

#include "../Utilities/Common.h"
#include "../Utilities/IO.h"
#include  "../Interrupts.h"
#include "Cartridges/Cartridge.h"
#include "IMU.h"
#include "PPU.h"
#include "Timer.h"

class Bus {
public:
	//DESTRUCTOR AND CONSTRUCTOR
	~Bus();

	//UPDATE COMPONENT POINTERS
	bool update_cartridge_ptr(Cartridge* cart);
	bool update_imu_ptr(IMU* imu);
	bool update_ppu_ptr(PPU* ppu);
	bool update_timer_ptr(Timer* timer);

	//MEMORY ACCESS AND REDIRECTION
	u8 read(u16 address);
	void write(u16 address, u8 value);

	u8 read_io(u16 address);
	void write_io(u16 address, u8 value);

private:
	//COMPONENT POINTERS
	Cartridge* m_cart = nullptr;
	IMU* m_imu = nullptr;
	PPU* m_ppu = nullptr;
	Timer* m_timer = nullptr;
};