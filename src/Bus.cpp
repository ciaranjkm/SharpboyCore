#include <Bus.h>
#include <Cartridges/Cartridge.h>

//PUBLIC FUNCTIONS

//DESTRUCTOR AND CONSTRUCTOR
Bus::~Bus() {
	m_cart = nullptr;
}

//UPDATE COMPONENT POINTERS
void Bus::update_cartridge_ptr(Cartridge* cart) {
	m_cart = cart;
}

//MEMORY ACCESS AND REDIRECTION
u8 Bus::read(u16 address) {
	return m_cart->read(address);
}

void Bus::write(u16 address, u8 value) {
	m_cart->write(address, value);
	return;
}