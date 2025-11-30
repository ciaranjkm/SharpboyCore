#pragma once

#include "../Common.h"
#include "../Components/IO.h"

#include <array>
#include <vector>
#include <queue>

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

enum e_fifo_state {
	fifo_inactive,
	fifo_fetch_tile_number,
	fifo_fetch_low,
	fifo_fetch_high,
	fifo_pushing
};

enum e_fifo_type {
	fifo_background,
	fifo_window,
	fifo_sprite,
	fifo_none,
};

struct s_basic_fifo {
	e_fifo_state current_state = fifo_fetch_tile_number;

	u8 tile_index = 0x00;
	u16 tile_address = 0x00;
	u8 tile_low = 0x00;
	u8 tile_high = 0x00;

	bool is_active() const { return (current_state != fifo_inactive); }
};

struct s_pixel {
	u8 colour = 0x00;
	u8 palette = 0x00;
	bool sprite = false;
};

struct s_ppu_fifos {
	s_basic_fifo background = {};
	s_basic_fifo window = {};
	s_basic_fifo sprite = {};

	bool is_window_fifo_active = false;
	int ticks = 0;
	int screen_x = 0;
	int fetcher_x = 0;
	
	bool dummy_fetch = false; //one background tile fetch before real fetches
	bool start_of_scanline = false;
	int discard = 0;
};

struct s_ppu_context {
	bool is_dma_active = false;
	bool is_frame_ready = false;

	int ticks = 0;
	int ticks_in_hblank = 0;
	e_ppu_mode current_mode = ppu_oam;

	u8 current_ly = 0x00;
	u8 current_scx = 0x00;
	u8 current_scy = 0x00;
};

//BASIC CONSTS
const int VRAM_SIZE = 0x2000;
const int OAM_SIZE = 0xa0;

const int DISPLAY_WIDTH = 160;
const int DISPLAY_HEIGHT = 144;
const int FRAME_BUFFER_SIZE = 160 * 144;

//DMA TIMING
const int DEFAULT_DMA_DELAY = 4;
const int DEFAULT_DMA_CYCLES = 160;

//PPU TIMING
const int SCANLINE_LENGTH = 456;
const int DOTS_PER_FRAME = 70224;
const int VBLANK_HEIGHT = 10;
const int VBLANK_DOT_TIME = DOTS_PER_FRAME - (SCANLINE_LENGTH * VBLANK_HEIGHT);

const int OAM_DURATION = 80;

const std::array<u32, 4> sb_colours = {
	0xffffffff,
	0x909090ff,
	0x505050ff,
	0x000000ff
};

class Bus;

class PPU {
public:
	PPU();

	//INITIALISATION
	bool set_bus_ptr(Bus* bus);
	void reset(bool using_boot_rom = false);

	//EXECUTION
	void tick();
	void dma_tick();

	//DRAWING
	void draw_scanline();

	bool get_frame_ready();
	void reset_frame_ready();
	
	std::array<u32, FRAME_BUFFER_SIZE>* get_frame_buffer();

	//MEMORY ACCESS
	u8 read(u16 address) const;
	void write(u16 address, u8 value);

	u8 read_io(u16 address) const;
	void write_io(u16 address, u8 value);

private:
	//MEMBER VARIABLES
	s_ppu_io m_ppu_io = {};
	s_dma m_dma = {};
	s_ppu_context m_ppu = {};

	s_ppu_fifos m_fifo = {};
	std::queue<u8> m_fifo_buffer = std::queue<u8>();

	//PTR TO BUS TO READ FOR DMA TRANSFER
	Bus* m_bus = nullptr;

	//DRAW DATA
	std::array<u32, FRAME_BUFFER_SIZE> m_completed_frame = std::array<u32, FRAME_BUFFER_SIZE>();
	std::array<u32, FRAME_BUFFER_SIZE> m_frame = std::array<u32, FRAME_BUFFER_SIZE>();

	//TIMING
	int m_ppu_ticks = 0;

	//DMA
	bool is_dma_active = false;

	//FIFO
	std::queue<s_pixel> background_fifo = std::queue<s_pixel>();

	//MEMORY
	std::vector<u8> m_vram = std::vector<u8>();
	std::array<u8, OAM_SIZE> m_oam = std::array<u8, OAM_SIZE>();

private:
	//MEMBER FUNCTIONS
	u8 bus_read(u16 address);

	//PPU MODE TICKS
	void oam_tick();
	void draw_tick();
	void hblank_tick();
	void vblank_tick();

	//INTERRUPTS + STAT
	void check_ly_lyc();
	void latch_start_line_values();

	//FIFO
	void tick_bg_fetcher();
	void fetcher_number();
	void fetcher_low();
	void fetcher_high();
	void fetcher_push();

	void output_pixels();
};