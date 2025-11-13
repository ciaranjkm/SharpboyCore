#include "../include/Bus.h"
#include "../include/Cartridges/Cartridge.h"

//PUBLIC FUNCTIONS

//DESTRUCTOR AND CONSTRUCTOR
Bus::~Bus() {
	m_cart = nullptr;
}

//UPDATE COMPONENT POINTERS
void Bus::update_cartridge_ptr(Cartridge* cart) {
	m_cart = cart;
}

//SINGLE STEP TEST MODE, INIT AND RESET
void Bus::set_sst_mode(Cartridge* cart) {
	m_cart = cart;
	sst_mode = true;
}

void Bus::reset_sst_mode() {
	m_cart = nullptr;
	sst_mode = false;
}

//MEMORY ACCESS AND REDIRECTION
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