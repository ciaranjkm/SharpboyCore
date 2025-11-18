#include <Components/Bus.h>

//DESTRUCTOR AND CONSTRUCTOR
Bus::~Bus() {
	m_cart = nullptr;
	m_imu = nullptr;
	m_ppu = nullptr;
	m_timer = nullptr;
}

//UPDATE COMPONENT POINTERS
bool Bus::update_cartridge_ptr(Cartridge* cart) {
	m_cart = cart;

	if (m_cart) {
		return true;
	}

	return false;
}

bool Bus::update_imu_ptr(IMU* imu) {
	m_imu = imu;

	if (m_imu) {
		return true;
	}

	return false;
}

bool Bus::update_ppu_ptr(PPU* ppu) {
	m_ppu = ppu;

	if (m_ppu) {
		return true;
	}
	
	return false;
}

bool Bus::update_timer_ptr(Timer* timer) {
	m_timer = timer;

	if (m_timer) {
		return true;
	}

	return false;
}

//MEMORY ACCESS AND REDIRECTION
u8 Bus::read(u16 address) {
	if (address >= 0x0000 && address < 0x8000) {
		return m_cart->read(address);
	}
	else if (address >= 0x8000 && address < 0xa000) {
		return m_ppu->read(address);
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
		return m_ppu->read(address);
	}
	else if ((address >= 0xff00 && address < 0xff80) || address == 0xffff) {		
		return read_io(address);
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
		m_ppu->write(address, value);
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
		m_ppu->write(address, value);
		return;
	}
	else if ((address >= 0xff00 && address < 0xff80) || address == 0xffff) {
		write_io(address, value);
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
	if (address == io_if) {
		return Interrupts::read_if();
	}
	else if (address == io_ie) {
		return Interrupts::read_ie();
	}
	else if (address >= io_joyp && address <= io_sc) {
		return m_imu->read_io(address);
	}
	else if (address >= io_div && address <= io_tac) {
		return m_timer->read_io(address);
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
		return m_ppu->read_io(address);
	}
	else if (address == io_bank) {
		return m_cart->read(address);
	}

	return 0xff;
}

void Bus::write_io(u16 address, u8 value) {
	if (address == io_if) {
		Interrupts::write_if(value);
	}
	else if (address == io_ie) {
		Interrupts::write_ie(value);
	}
	else if (address >= io_joyp && address <= io_sc || address == io_if || address == io_ie) {
		m_imu->write_io(address, value);
		return;
	}
	else if (address >= io_div && address <= io_tac) {
		m_timer->write_io(address, value);
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
		m_ppu->write_io(address, value);
	}
	else if (address == io_bank) {
		m_cart->write(address, value);
	}
}