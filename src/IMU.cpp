#include <IMU.h>

IMU::IMU() {
	m_wram.resize(WRAM_SIZE);
	m_hram.resize(HRAM_SIZE);
}

//RESET
void IMU::reset() {
	m_wram.clear();
	m_hram.clear();

	m_io = {};
}

//MEMORY ACCESS
u8 IMU::read(u16 address) {
	if (address >= 0xc000 && address < 0xe000) {
		return m_wram[(u16)(address - 0xc000)];
	}
	else if (address >= 0xe000 && address < 0xfe00) {
		return read((u16)(address - 0x2000));
	}
	else if (address >= 0xff80 && address < 0xffff) {
		return m_hram[(u16)(address - 0xff80)];
	}
	else {
		return 0xff;
	}
}

void IMU::write(u16 address, u8 value) {
	if (address >= 0xc000 && address < 0xe000) {
		m_wram[(u16)(address - 0xc000)] = value;
	}
	else if (address >= 0xe000 && address < 0xfe00) {
		write((u16)(address - 0x2000), value);
	}
	else if (address >= 0xff80 && address < 0xffff) {
		m_hram[(u16)(address - 0xff80)] = value;
	}
	else {
		return;
	}
}

u8 IMU::read_io(u16 address) {
	switch (address) {
	case io_ie:
		return m_io.IE & 0x1f;

	case io_joyp:
		return convert_to_joypad_value(m_joypad_state);

	case io_sb:
		return m_io.sb;

	case io_sc:
		return m_io.sc;

	default:
		return 0xff;
	}
}

void IMU::write_io(u16 address, u8 value) {
	switch (address) {
	case io_ie:
		m_io.IE = value & 0x1f;
		return;

	case io_sb:
		m_io.sb = value;
		return;

	case io_sc:
		m_io.sc = value;
		if (value & 0x80) {
			char c = static_cast<char>(m_io.sb);
			std::cout << c;
			std::cout.flush();


			m_io.sc &= ~0x80;
		}
		break;

	default:
		return;
	}

}

//UPDATE JOYPAD STATE
void IMU::update_joypad(s_joypad_state new_joypad_state) {
	m_joypad_state = new_joypad_state;
}