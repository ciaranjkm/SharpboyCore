#include <Components/PPU.h>
#include <Components/Bus.h>

//CONSTRUCTOR DESTRUCTOR
PPU::PPU() {
    m_draw_data = std::make_unique<s_draw_data>();
    m_memory = std::make_unique<s_ppu_memory>();
}

PPU::~PPU() {
    m_draw_data.reset();
    m_memory.reset();
}

//INITIALISTION
bool PPU::set_bus_ptr(Bus* bus) {
    m_bus = bus;

    if (m_bus) {
        return true;
    }
    
    return false;
}

void PPU::reset(bool using_boot_rom) {
    if (!using_boot_rom) {
        m_ppu_io = {};
        m_ppu_io.lcdc = 0x91;
        m_ppu_io.stat = 0x85;
        m_ppu_io.bgp = 0xfc;
        m_ppu_io.obp0 = 0xff;
        m_ppu_io.obp1 = 0xff;
    }
    else {
        m_ppu_io = {};
    }

    m_memory->vram.fill(0x00);
    m_memory->oam.fill(0x00);

    m_draw_data->clear_frame(m_draw_data->completed_frame);
    m_draw_data->clear_frame(m_draw_data->frame);

    m_dma = {};
    m_bg_fifo.reset_for_new_scanline();
}

//EXECUTION
void PPU::tick() {
    ppu_tick();
    //RUN ONE TICK IN THE CURRENT MODE

    //INCREMENT T CYCLE COUNT

    //IF COMPLETED MODE, SWITCH TO NEXT MODE

    m_ppu.t_cycles++;
    switch_mode();
}

void PPU::dma_tick() {
    bool started_new_this_tick = false;

    if (m_dma.start_new) {
        m_dma.ticks_since_start++;

        //ALLIGN TO CPU CLOCK + 1 M CYCLE
        if (m_dma.ticks_since_start == DEFAULT_DMA_DELAY + 1) {
            if (!m_dma.active) {
                m_dma.active = true;
            }

            started_new_this_tick = true;
        }
    }

    if (m_dma.active) {
        m_dma.ticks_this_cycle++;
        if (m_dma.ticks_this_cycle == DEFAULT_DMA_DELAY) {

            m_dma.ticks_this_cycle = 0;
            m_dma.cycles_this_transfer++;

            u8 value = m_bus->unblocked_read(m_dma.dma_address++);
            int oam_target = m_dma.cycles_this_transfer - 1;

            m_memory->oam[oam_target] = value;
        }

        if (m_dma.cycles_this_transfer >= DEFAULT_DMA_CYCLES) {
            m_dma = {};
            m_dma.active = false;
        }
    }

    if (started_new_this_tick) {
        m_dma.start_new = false;
        m_dma.ticks_this_cycle = 0;
        m_dma.cycles_this_transfer = 0;

        m_dma.dma_address = (m_dma.start_byte << 8) & 0xff00;
    }
}

//DRAW DATA GET AND RESET
bool PPU::get_frame_ready() const  {
    if (!m_draw_data) {
        i_Logger->Log("PPU no draw data pointer!", LOGGER_PR_WARNING);
        return false;
    }

    return m_draw_data->get_frame_ready();
}

void PPU::reset_frame_ready() {
    if (!m_draw_data) {
        i_Logger->Log("PPU no draw data pointer!", LOGGER_PR_WARNING);
        return;
    }

    m_draw_data->reset_frame_ready();
}

std::array<u32, FRAME_BUFFER_SIZE>* PPU::get_frame_buffer() {
    return &m_draw_data->completed_frame;
}

std::array<u8, VRAM_SIZE>* PPU::get_vram() {
    return &m_memory->vram;
}

//MEMORY ACCESS
u8 PPU::read(u16 address) const {
    if (address >= 0x8000 && address < 0xa000) {
        return m_memory->vram[u16(address - 0x8000)];
    }
    else if (address >= 0xfe00 && address < 0xfea0) {
        if (m_dma.active) {
            return 0xff;
        }

        return m_memory->oam[u16(address - 0xfe00)];
    }
    
    return 0xff;
}

void PPU::write(u16 address, u8 value) {
    if (address >= 0x8000 && address < 0xa000) {
        m_memory->vram[u16(address - 0x8000)] = value;
    }
    else if (address >= 0xfe00 && address < 0xfea0) {
        if (m_dma.active) {
            return;
        }

        m_memory->oam[u16(address - 0xfe00)] = value;
    }
}

u8 PPU::read_io(u16 address) const  {
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
        return m_ppu_io.ly;
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
        m_ppu_io.stat = value & 0xfc;
        m_ppu_io.stat |= 0x80;
        return;
    case io_scy:
        m_ppu_io.scy = value;
        return;
    case io_scx:
        m_ppu_io.scx = value;
        return;
    case io_lyc:
        m_ppu_io.lyc = value;
        check_ly_lyc();
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

    case io_dma:
        m_ppu_io.dma = value;
        m_dma.start_byte = value;

        m_dma.start_new = true;
        m_dma.ticks_since_start = 0;

        return;

    default:
        return ;
    }

}

//MEMBER FUNCTIONS
u8 PPU::bus_read(u16 address) {
    return m_bus->unblocked_read(address);
}

//PPU MODE TICKS
void PPU::ppu_tick() {
    switch (m_ppu.mode) {
    case ppu_oam:
        oam_tick();
        return;
    case ppu_draw:
        draw_tick();
        return;
    case ppu_hblank:
        hblank_tick();
        return;
    case ppu_vblank:
        vblank_tick();
        return;
    default:
        i_Logger->Log("PPU mode invalid!", LOGGER_PR_ERROR);
        return;
    }
}

void PPU::switch_mode() {
    //DECIDE WHAT MODE TO SWITCH TO
    e_ppu_mode new_mode = m_ppu.mode;
    switch (m_ppu.mode) {
    case ppu_oam:
        if (m_ppu.t_cycles >= OAM_DURATION) {
            new_mode = ppu_draw;

            //std::cout << "switched to draw " << m_ppu.t_cycles << "\n";
        }
        else {
            return;
        }
        break;

    case ppu_draw:
        if (m_ppu.t_cycles >= 172) {
            new_mode = ppu_hblank;
            m_ppu.t_cycles_in_hblank = SCANLINE_LENGTH - (m_ppu.t_cycles);

            //std::cout << "switched to hblank " << m_ppu.t_cycles << "\n";
        }
        else {
            return;
        }
        break;

    case ppu_hblank:
        if (m_ppu.t_cycles >= m_ppu.t_cycles_in_hblank) {
            if (++m_ppu_io.ly >= DISPLAY_HEIGHT) {
                new_mode = ppu_vblank;

                std::cout << "switched to vblank " << m_ppu.t_cycles << "\n";
            }
            else {
                new_mode = ppu_oam;          

                //std::cout << "switched to oam " << m_ppu.t_cycles << "\n";
            }

            m_ppu.reset_for_new_scanline();
            m_bg_fifo.reset_for_new_scanline();

            m_ppu.latch_values(m_ppu_io);
        }
        else {
            return;
        }

        break;

    case ppu_vblank:
        if (m_ppu.t_cycles >= SCANLINE_LENGTH) {
            if (++m_ppu_io.ly >= DISPLAY_HEIGHT + VBLANK_HEIGHT) {
                new_mode = ppu_oam;
                m_ppu_io.ly = 0x00;
            }

            m_ppu.reset_for_new_scanline();
			m_ppu.latch_values(m_ppu_io);
        }
        break;
    }
   
    //UPDATE STAT REGISTER 
    u8& stat = m_ppu_io.stat;
    u8 top_stat = stat & 0b11111100;
    stat = top_stat | u8(new_mode);

    //CHECK FOR STAT INTERRUPT 
    if (stat_irq_pending()) {
        Interrupts::send_interrupt(interrupt_lcd);
    }

    m_ppu.mode = new_mode;
}

bool PPU::stat_irq_pending() {
    

    return false;
}

void PPU::oam_tick() {

}

void PPU::draw_tick() {

}

void PPU::hblank_tick() {

}

void PPU::vblank_tick() {

}

//INTERRUPTS + STAT

void PPU::check_ly_lyc() {
    if (m_ppu_io.ly == m_ppu_io.lyc) {
        Interrupts::send_interrupt(interrupt_lcd);
    }
}

//FIFO BG/SPRITE
void PPU::tick_bg_fetcher() {
    m_bg_fifo.fetcher_t_cycles++;
    if (m_bg_fifo.fetcher_t_cycles % 2 != 0) {
        return;
    }

    switch (m_bg_fifo.current_state) {
    case fifo_fetch_tile_number:
        fetcher_number();
        m_bg_fifo.current_state = fifo_fetch_low;
        return;

    case fifo_fetch_low:
        fetcher_low();
        m_bg_fifo.current_state = fifo_fetch_high;
        return;

    case fifo_fetch_high:
        fetcher_high();
        m_bg_fifo.current_state = fifo_pushing;
        return;

    case fifo_pushing:
        fetcher_push();
        m_bg_fifo.current_state = fifo_fetch_tile_number;
        return;

    default:
        return;
    }

}

void PPU::fetcher_number() {
    u16 tile_id_base = 0x9800;
    if (m_ppu_io.lcdc & 0x08) {
        tile_id_base = 0x9c00;
    }

    u8 ly = m_ppu.c_ly;
    u8 scx = (m_ppu_io.scx & 0xf8) | (m_ppu.c_scx & 0x07);
    u8 scy = m_ppu_io.scy;

    u8 ly_scy_offset = ((ly + scy) / 8) & 0x1f;
    u8 lx_scx_offset = ((m_bg_fifo.fx_position + scx) / 8) & 0x1f;

    u16 tile_id_address = tile_id_base | (ly_scy_offset << 5) | lx_scx_offset;

    u8 tile_index = m_memory->vram[u16(tile_id_address - 0x8000)];

    u16 tile_data_base = 0x8000;
    bool bit_twelve = !((m_ppu_io.lcdc & 0x10) || (tile_index & 0x80));
    if (bit_twelve) {
        tile_data_base |= (1 << 12);
    }

    tile_data_base |= (tile_index << 4);
    tile_data_base |= ((m_ppu_io.ly + m_ppu_io.scy) % 8) << 1;

    m_bg_fifo.tile_address = tile_data_base;
}

void PPU::fetcher_low() {
    m_bg_fifo.tile_low = m_memory->vram[u16(m_bg_fifo.tile_address - 0x8000)];
}

void PPU::fetcher_high() {
    m_bg_fifo.tile_high = m_memory->vram[u16((m_bg_fifo.tile_address + 1) - 0x8000)];
}

void PPU::fetcher_push() {
    if (m_bg_fifo.pixel_queue.empty()) {
        for (int b = 7; b >= 0; b--) {
            u8 low = (m_bg_fifo.tile_low >> b) & 0x01;
            u8 high = (m_bg_fifo.tile_high >> b) & 0x01;

            u8 colour = (high << 1) | low;

            s_pixel p = {
                .colour = colour,
                .palette = m_ppu_io.bgp,
                .sprite = false
            };

            m_bg_fifo.push_pixel(p);
            m_bg_fifo.fx_position++;
        }
    }
}

void PPU::output_pixels() {
    if (!m_bg_fifo.pixel_queue.empty()) {
        /*if (m_fifo.start_of_scanline) {
            m_fifo.discard = m_ppu.current_scx % 8;
            m_fifo.start_of_scanline = false;
            
            if (m_fifo.discard > 0) {
                for (int i = m_fifo.discard; i > 0; i--) {
                    if (!background_fifo.empty()) {
                        background_fifo.pop();
                    }
                }				
                return;
            }
        }*/

        s_pixel out = {};
        if (!m_bg_fifo.pop_pixel(out)) {
            return;
        }

        int shift = out.colour * 2;
        int colour = (m_ppu_io.bgp >> shift) & 0x03;

        m_draw_data->frame[m_ppu_io.ly * DISPLAY_WIDTH + m_bg_fifo.sx_position++] = DISPLAY_COLOURS[colour];
    }
}