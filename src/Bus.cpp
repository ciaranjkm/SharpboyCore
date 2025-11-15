#include <Bus.h>

//DESTRUCTOR AND CONSTRUCTOR
Bus::~Bus() {
	m_cart = nullptr;
	m_imu = nullptr;
	m_ppu = nullptr;
}

//UPDATE COMPONENT POINTERS
void Bus::update_cartridge_ptr(Cartridge* cart) {
	m_cart = cart;
}

void Bus::update_imu_ptr(IMU* imu) {
	m_imu = imu;
}

void Bus::update_ppu_ptr(PPU* ppu) {
	m_ppu = ppu;
}

//MEMORY ACCESS AND REDIRECTION
u8 Bus::read(u16 address) {
	if (address >= 0x0000 && address < 0x8000) {
		return m_cart->read(address);
	}
	else if (address >= 0x8000 && address < 0xa000) {
		//ppu read vram
		return 0xff;
	}
	else if (address >= 0xa000 && address < 0xc000) {
		return m_cart->read(address);
	}
	else if (address >= 0xc000 && address < 0xe000) {
		return m_imu->read(address);
	}
	else if (address >= 0xe000 && address < 0xfe00) {
		return m_imu->read((u16)(address - 0x2000));
	}
	else if (address >= 0xfe00 && address < 0xfea0) {
		//ppu read oam
		return 0xff;
	}
	else if ((address >= 0xff00 && address < 0xff80) || address == 0xffff) {		
		//io read
		return 0xff;
	}
	else if (address >= 0xff80 && address < 0xffff) {
		return m_imu->read(address);
	}
	else {
		return 0xff;
	}
}

void Bus::write(u16 address, u8 value) {
	if (address >= 0x0000 && address < 0x8000) {
		return;
	}
	else if (address >= 0x8000 && address < 0xa000) {
		//ppu write vram
		return;
	}
	else if (address >= 0xa000 && address < 0xc000) {
		m_cart->write(address, value);
	}
	else if (address >= 0xc000 && address < 0xe000) {
		m_imu->write(address, value);
		return;
	}
	else if (address >= 0xe000 && address < 0xfe00) {
		m_imu->write((u16)(address - 0x2000), value);
		return;
	}
	else if (address >= 0xfe00 && address < 0xfea0) {
		//ppu write oam
		return;
	}
	else if ((address >= 0xff00 && address < 0xff80) || address == 0xffff) {
		//io write
		return;
	}
	else if (address >= 0xff80 && address < 0xffff) {
		m_imu->write(address, value);
		return;
	}
	else {
		return;
	}

}

u8 Bus::read_io(u16 address) {
	//joyp, serial, interrupt
	if (address >= io_joyp && address <= io_sc || address == io_if) {
		return m_imu->read(address);
	}
	else if (address >= io_div && address <= io_tac) {
		//timer io read
		//return timer->read(address);
	}
	else if (address >= io_nr10 && address <= io_nr52) {
		//audio io read
		//return audio->read(address);
	}
	else if (address >= 0xff30 && address <= 0xff3f) {
		//audio wave io read
		//return audio->read(address);
	}
	else if (address >= io_lcdc && address <= io_wx) {
		//ppu io read
		//return ppu->read(address);
	}
	else if (address == io_bank) {
		return m_cart->read(address);
	}

	return 0xff;
}

void Bus::write_io(u16 address, u8 value) {
	//joyp, serial, interrupt
	if (address >= io_joyp && address <= io_sc || address == io_if) {
		m_imu->write(address, value);
		return;
	}
	else if (address >= io_div && address <= io_tac) {
		//timer io write
		//return timer->write(address);
	}
	else if (address >= io_nr10 && address <= io_nr52) {
		//audio io write
		//return audio->write(address);
	}
	else if (address >= 0xff30 && address <= 0xff3f) {
		//audio wave io write
		//return audio->write(address);
	}
	else if (address >= io_lcdc && address <= io_wx) {
		//ppu io write
		//return ppu->write(address);
	}
	else if (address == io_bank) {
		m_cart->write(address, value);
	}
}