#pragma once
#include "Utilities/Common.h"

class Cartridge;

class Bus {
public:
	//DESTRUCTOR AND CONSTRUCTOR
	~Bus();

	//UPDATE COMPONENT POINTERS
	void update_cartridge_ptr(Cartridge* cart);

	//SINGLE STEP TEST MODE, INIT AND RESET
	void set_sst_mode(Cartridge* cart);
	void reset_sst_mode();

	//MEMORY ACCESS AND REDIRECTION
	u8 read(u16 address);
	void write(u16 address, u8 value);

private:
	//SST FLAG
	bool sst_mode = false;

	//COMPONENT POINTERS
	Cartridge* m_cart = nullptr;
};