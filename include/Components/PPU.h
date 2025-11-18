#pragma once

#include "../Utilities/Common.h"
#include "../Utilities/IO.h"

#include <array>
#include <vector>

enum e_ppu_mode {
	ppu_idle,
	ppu_oam,
	ppu_draw,
	ppu_hblank,
	ppu_vblank
};

struct s_ppu_io {
	u8 lcdc = 0x00;
	u8 stat = 0x00;
	u8 scy = 0x00;
	u8 scx = 0x00;
	u8 ly = 0x00;
	u8 lyc = 0x00;
	u8 dma = 0x00;
	u8 bgp = 0x00;
	u8 obp0 = 0x00;
	u8 obp1 = 0x00;
	u8 wy = 0x00;
	u8 wx = 0x00;
};

const int VRAM_SIZE = 0x2000;
const int OAM_SIZE = 0xa0;

const int SCANLINE_LENGTH = 456;

class PPU {
public:
	PPU();

	void tick();
	void reset(bool using_boot_rom = false);

	u8 read(u16 address);
	void write(u16 address, u8 value);

	u8 read_io(u16 address);
	void write_io(u16 address, u8 value);

private:
	s_ppu_io m_ppu_io = {};

	std::vector<u8> m_vram = std::vector<u8>();
	std::array<u8, OAM_SIZE> m_oam = std::array<u8, OAM_SIZE>();
};