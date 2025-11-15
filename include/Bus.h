#pragma once
#include "Utilities/Common.h"
#include "Cartridges/Cartridge.h"
#include "IMU.h"

class Bus {
public:
	//DESTRUCTOR AND CONSTRUCTOR
	~Bus();

	//UPDATE COMPONENT POINTERS
	void update_cartridge_ptr(Cartridge* cart);
	void update_imu_ptr(IMU* imu);

	//MEMORY ACCESS AND REDIRECTION
	u8 read(u16 address);
	void write(u16 address, u8 value);

private:
	//COMPONENT POINTERS
	Cartridge* m_cart = nullptr;
	IMU* m_imu = nullptr;
};