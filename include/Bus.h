#pragma once
#include "Utilities/Common.h"

class Cartridge;

class Bus {
public:
	~Bus();

	bool set_cart_ptr(Cartridge* cart);

	void set_sst_mode();
	void reset_sst_mode();

	u8 read(u16 address);
	void write(u16 address, u8 value);

private:
	bool sst_mode = false;
	Cartridge* m_cart = nullptr;
};