#pragma once

#include "../Common.h"
#include "../Components/IO.h"

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

struct s_dma {
	bool active = false;

	bool start_new = false;
	int ticks_since_start = 0;

	int ticks_this_cycle = 0;
	int cycles_this_transfer = -1;

	u16 dma_address = 0x0000;
	u8 start_byte = 0x00;
};

const int VRAM_SIZE = 0x2000;
const int OAM_SIZE = 0xa0;

const int SCANLINE_LENGTH = 456;

const int DEFAULT_DMA_DELAY = 4;
const int DEFAULT_DMA_CYCLES = 160;

class Bus;

class PPU {
public:
	PPU();

	bool set_bus_ptr(Bus* bus);

	void tick();
	void dma_tick();

	void reset(bool using_boot_rom = false);

	u8 read(u16 address);
	void write(u16 address, u8 value);

	u8 read_io(u16 address);
	void write_io(u16 address, u8 value);

private:
	s_ppu_io m_ppu_io = {};
	s_dma m_dma = {};

	Bus* m_bus = nullptr;

	//PPU TIMING
	const int DOTS_PER_FRAME = 70224;
	const int SCALINE_LENGTH = 456;
	const int DISPLAY_WIDTH = 160;
	const int DISPLAY_HEIGHT = 144;
	const int VBLANK_HEIGHT = 10;

	const int VBLANK_DOT_TIME = DOTS_PER_FRAME - (SCANLINE_LENGTH * VBLANK_HEIGHT);
	int m_ppu_ticks = 0;

	//DMA
	bool is_dma_active = false;

	std::vector<u8> m_vram = std::vector<u8>();
	std::array<u8, OAM_SIZE> m_oam = std::array<u8, OAM_SIZE>();

private:
	u8 bus_read(u16 address);
};