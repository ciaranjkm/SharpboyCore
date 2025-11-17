#include <PPU.h>

PPU::PPU() {
	m_vram.resize(VRAM_SIZE);
}

void PPU::reset() {
	m_vram.clear();
}

void PPU::tick() {
	//not impl yet
}

u8 PPU::read(u16 address) {
	if (address >= io_lcdc && address <= io_wx) {
		return read_io(address);
	}
	else if(address >= 0x8000 && address < 0xa000)  {
		return m_vram[(u16)(address - 0x8000)];
	}
	else if (address >= 0xe000 && address < 0xfea0) {
		return m_oam[(u16)(address - 0xe000)];
	}
	else {
		return 0xff;
	}
}

void PPU::write(u16 address, u8 value) {
	if (address >= io_lcdc && address <= io_wx) {
		write_io(address, value);
		return;
	}
	else if (address >= 0x8000 && address < 0xa000) {
		m_vram[(u16)(address - 0x8000)] = value;
		return;
	}
	else if (address >= 0xe000 && address < 0xfea0) {
		m_oam[(u16)(address - 0xe000)] = value;
		return;
	}
	else {
		return;
	}
}

u8 PPU::read_io(u16 address) {
	switch (address) {
	case io_lcdc:
		return m_ppu_io.lcdc;
	case io_stat:
		return m_ppu_io.stat;
	case io_scy:
		return m_ppu_io.scy;
	case io_scx:
		return m_ppu_io.scx;
	case io_ly:
		return 0x90; // return 0x90 for testing at the moment
		//return m_ppu_io.ly;
	case io_lyc:
		return m_ppu_io.lyc;
	case io_dma:
		return m_ppu_io.dma;
	case io_bgp:
		return m_ppu_io.bgp;
	case io_obp0:
		return m_ppu_io.obp0;
	case io_obp1:
		return m_ppu_io.obp1;
	case io_wy:
		return m_ppu_io.wy;
	case io_wx:
		return m_ppu_io.wx;
	default:
		return 0xff;
	}
}

void PPU::write_io(u16 address, u8 value) {
	switch (address) {
	case io_lcdc:
		m_ppu_io.lcdc = value;
		return;
	case io_stat:
		m_ppu_io.stat = value;
		return;
	case io_scy:
		m_ppu_io.scy = value;
		return;
	case io_scx:
		m_ppu_io.scx = value;
		return;
	case io_ly:
		m_ppu_io.ly = value;
		return;
	case io_lyc:
		m_ppu_io.lyc = value;
		return;
	case io_dma:
		m_ppu_io.dma = value;
		return;
	case io_bgp:
		m_ppu_io.bgp = value;
		return;
	case io_obp0:
		m_ppu_io.obp0 = value;
		return;
	case io_obp1:
		m_ppu_io.obp1 = value;
		return;
	case io_wy:
		m_ppu_io.wy = value;
		return;
	case io_wx:
		m_ppu_io.wx = value;
		return;
	default:
		return ;
	}

}