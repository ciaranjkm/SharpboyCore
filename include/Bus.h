#pragma once
#include "Utilities/Common.h"

class Cartridge;

class Bus {
public:
	//DESTRUCTOR AND CONSTRUCTOR
	~Bus();

	//UPDATE COMPONENT POINTERS
	void update_cartridge_ptr(Cartridge* cart);

	//MEMORY ACCESS AND REDIRECTION
	u8 read(u16 address);
	void write(u16 address, u8 value);

private:
	//COMPONENT POINTERS
	Cartridge* m_cart = nullptr;
};