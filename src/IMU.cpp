#include <IMU.h>

void IMU::reset() {
	m_wram.fill(0x00);
	m_hram.fill(0x00);

	m_io = {};
}

u8 IMU::read(u16 address) {
	if (address >= 0xc000 && address < 0xe000) {
		return m_wram[(u16)(address - 0xc000)];
	}
	else if (address >= 0xe000 && address < 0xfe00) {
		return read((u16)(address - 0x2000));
	}
	else if ((address >= 0xff00 && address < 0xff80) || address == 0xffff) {
		return read_io(address);
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
	else if ((address >= 0xff00 && address < 0xff80) || address == 0xffff) {
		write_io(address, value);
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
	case 0xffff:
		return m_io.IE & 0x1f;

	default:
		return 0xff;
	}
}

void IMU::write_io(u16 address, u8 value) {
	switch (address) {
	case 0xffff:
		m_io.IE = value & 0x1f;
		return;

	default:
		return;
	}

}