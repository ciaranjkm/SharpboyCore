#pragma once

#include "../Utilities/SBLogger.h"
#include "../Common.h"

#include <array>
#include <queue>

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
const int OAM_DURATION = 80;
const int VBLANK_HEIGHT = 10;

//DISPLAY COLOURS (ARGB32)
const std::array<u32, 4> DISPLAY_COLOURS = {
    0xffffffff,
    0x909090ff,
    0x505050ff,
    0x000000ff
};

//PPU MODES
enum e_ppu_mode {
    ppu_idle = -1,
    ppu_oam = 2,
    ppu_draw = 3,
    ppu_hblank = 0,
    ppu_vblank = 1
};

//IO REGISTERS + DMA + FIFO STRUCT
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

struct s_pixel {
    u8 colour = 0x00;
    u8 palette = 0x00;
    bool sprite = false;
};

struct s_basic_fifo {
    /*   Fetcher State   */
    e_fifo_state current_state = fifo_fetch_tile_number;

    /*   Target Tile Data   */
    u16 tile_address = 0x00;
    u8 tile_low = 0x00;
    u8 tile_high = 0x00;

    /*   Fifo Operation   */
    int fetcher_t_cycles = 0;
    int fx_position = 0;
    int sx_position = 0;
    bool is_window = false;

    /*   Queue Functions   */
    std::queue<s_pixel> pixel_queue = std::queue<s_pixel>();

    bool push_pixel(s_pixel p) {
        if (pixel_queue.size() <= 8) {
            pixel_queue.push(p);
            return true;
        }
        return false;
    }
    bool pop_pixel(s_pixel& p) {
        if (!pixel_queue.empty()) {
            p = pixel_queue.front();
            pixel_queue.pop();

            return true;
        }
        return false;
    }
    void clear_pixel_queue() {
        while (!pixel_queue.empty()) {
            pixel_queue.pop();
        }
    }

    /*   Reset Fifo   */
    void reset_for_new_scanline() {
        current_state = fifo_fetch_tile_number;
        
        tile_address = 0x00;
        tile_high = 0x00;
        tile_low = 0x00;

        fetcher_t_cycles = 0;
        fx_position = 0;
        is_window = false;

        clear_pixel_queue();
    }
};

//PPU DRAW DATA 
struct s_draw_data {
    bool is_frame_ready = false;

    void reset_frame_ready() {
        is_frame_ready = false;
    }

    bool get_frame_ready() {
        return is_frame_ready;
    }

    void clear_frame(std::array<u32, 160 * 144>& target_frame) {
        target_frame.fill(0x00);
    }

    std::array<u32, 160 * 144> completed_frame = std::array<u32, 160 * 144>();
    std::array<u32, 160 * 144> frame = std::array<u32, 160 * 144>();
};

//PPU MEMORY (OAM AND VRAM)
struct s_ppu_memory {
    std::array<u8, VRAM_SIZE> vram = std::array<u8, VRAM_SIZE>();
    std::array<u8, OAM_SIZE> oam = std::array<u8, OAM_SIZE>();
};

struct s_ppu_context {
    /*   Latched Values   */
    u8 c_ly = 0x00;
    u8 c_scy = 0x00;
    u8 c_scx = 0x00;

    /*   Current Scanline   */
    int t_cycles = 0;
    int t_cycles_in_hblank = 0;
    bool draw_complete = false;
    e_ppu_mode mode = ppu_oam;

    /*   Reset For New Scanline   */
    void reset_for_new_scanline() {
        t_cycles = 0;
        t_cycles_in_hblank = 0;
        draw_complete = false;
        mode = ppu_oam;
	}

    void latch_values(const s_ppu_io& ppu_io) {
        c_ly = ppu_io.ly;
        c_scy = ppu_io.scy;
        c_scx = ppu_io.scx;
	}
};

class Bus;

class PPU {
public:
    //CONSTRUCTOR DESTRUCTOR
    PPU();
    ~PPU();

    //INITIALISATION
    bool set_bus_ptr(Bus* bus);
    void reset(bool using_boot_rom = false);

    //EXECUTION
    void tick();
    void dma_tick();

    //DRAW DATA GET AND RESET
    bool get_frame_ready() const;
    void reset_frame_ready();
    
    std::array<u32, FRAME_BUFFER_SIZE>* get_frame_buffer();
    std::array<u8, VRAM_SIZE>* get_vram();

    //MEMORY ACCESS
    u8 read(u16 address) const;
    void write(u16 address, u8 value);

    u8 read_io(u16 address) const;
    void write_io(u16 address, u8 value);

private:
    //CONTEXT VARIABLES
    s_ppu_io m_ppu_io = {};
    s_dma m_dma = {};
    s_ppu_context m_ppu = {};

    //FIFO + DRAWING MEMBER VARIABLES
    s_basic_fifo m_bg_fifo = {};

    std::unique_ptr<s_draw_data> m_draw_data = nullptr;

    //MEMORY ACCESS VARIABLES
    Bus* m_bus = nullptr;
    std::unique_ptr<s_ppu_memory> m_memory = nullptr;

private:
    //MEMBER FUNCTIONS
    u8 bus_read(u16 address);

    //PPU MODE TICKS
    void ppu_tick();
    void switch_mode();
    bool stat_irq_pending();

    void oam_tick();
    void draw_tick();
    void hblank_tick();
    void vblank_tick();

    //INTERRUPTS + STAT
    void check_ly_lyc();

    //FIFO BG/SPRITE 
    //todo modify this to decide what fifo to use and grab a ref to it
    void tick_bg_fetcher();
    void fetcher_number();
    void fetcher_low();
    void fetcher_high();
    void fetcher_push();
    void output_pixels();
};