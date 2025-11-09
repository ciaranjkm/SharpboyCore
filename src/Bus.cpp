#include "../include/Bus.h"
#include "../include/Cartridges/Cartridge.h"

Bus::~Bus() {
	m_cart = nullptr;
}

bool Bus::set_cart_ptr(Cartridge* cart) {
	m_cart = cart;

	if (!m_cart) {
		return false;
	}

	return true;
}

void Bus::set_sst_mode() {
	sst_mode = true;
}

void Bus::reset_sst_mode() {
	sst_mode = false;
}

u8 Bus::read(u16 address) {
	if (sst_mode) {
		return m_cart->read(address);
	}
}

void Bus::write(u16 address, u8 value) {
	if (sst_mode) {
		m_cart->write(address, value);
		return;
	}
}