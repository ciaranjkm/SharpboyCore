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

	m_draw_data.reset();
	m_draw_data = std::make_unique<s_draw_data>();

	m_dma = {};
	m_fifo = {};
}

//EXECUTION
void PPU::tick() {
	m_ppu.ticks++;

	switch (m_ppu.current_mode) {
	case ppu_oam:
		oam_tick(); //SCAN OAM FOR SPRITES TO BE DRAWN ON THE SCANLINE
		break;

	case ppu_draw:
		draw_tick(); //TICK FIFO AND PUSH PIXELS
		break;

	case ppu_hblank:
		hblank_tick(); //IDLE
		break;

	case ppu_vblank:
		vblank_tick(); //IDLE
		break;

	default:
		//THIS SHOULD NOT HAPPEN
		break;
	}
}

void PPU::dma_tick() {
	bool started_new_this_tick = false;

	if (m_dma.start_new) {
		m_dma.ticks_since_start++;

		//ALLIGN TO CPU CLOCK + 1 M CYCLE
		if (m_dma.ticks_since_start == DEFAULT_DMA_DELAY + 2) {
			if (!m_dma.active) {
				m_dma.active = true;
				m_bus->dma_start();
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
			m_bus->dma_end();
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
	return m_ppu.is_frame_ready;
}

void PPU::reset_frame_ready() {
	m_ppu.is_frame_ready = false;
}

std::array<u32, FRAME_BUFFER_SIZE>* PPU::get_frame_buffer() {
	return &m_draw_data->completed_frame;
}

//GET VRAM HERE

//MEMORY ACCESS
u8 PPU::read(u16 address) const {
	if (address >= 0x8000 && address < 0xa000) {
		return m_memory->vram[u16(address - 0x8000)];

		/*
		//only allow reads during hblank and vblank
		if (m_ppu.current_mode == ppu_draw) {
			return 0xff;
		}
		else {
			return m_vram[u16(address - 0x8000)];
		}
		*/
	}
	else if (address >= 0xfe00 && address < 0xfea0) {
		if (m_dma.active) {
			return 0xff;
		}

		return m_memory->oam[u16(address - 0xfe00)];

		/*
		if ((m_ppu.current_mode == ppu_oam) || (m_ppu.current_mode == ppu_draw) || (m_dma.active)){
			return 0xff;
		}
		else {
			return m_oam[u16(address - 0xfe00)];
		}
		*/
	}
	
	return 0xff;
}

void PPU::write(u16 address, u8 value) {
	if (address >= 0x8000 && address < 0xa000) {
		m_memory->vram[(u16)(address - 0x8000)] = value;

		/*
		if (m_ppu.current_mode == ppu_draw) {
			return;
		}
		else {
			m_vram[(u16)(address - 0x8000)] = value;
			return;
		}
		*/
	}
	else if (address >= 0xfe00 && address < 0xfea0) {
		if (m_dma.active) {
			return;
		}

		m_memory->oam[(u16)(address - 0xfe00)] = value;

		/*
		if ((m_ppu.current_mode == ppu_oam) || (m_ppu.current_mode == ppu_draw) || (m_dma.active)) {
			return;
		}
		else {
			m_oam[(u16)(address - 0xfe00)] = value;
			return;
		}
		*/
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
		m_dma.start_new = true;
		m_dma.start_byte = value;
		m_dma.ticks_since_start = 0;

		m_ppu_io.dma = value;

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
void PPU::oam_tick() {
	if (m_ppu.ticks == 4) {
		check_ly_lyc();
	}

	//do nothing for now just wait for mode change
	if (m_ppu.ticks == OAM_DURATION) {
		m_ppu.ticks -= OAM_DURATION;
		m_ppu.current_mode = ppu_draw;

		//clear + reset fifo for the next line
		m_fifo.dummy_fetch = true;
		m_fifo.discard_fetch = true;
		m_fifo.start_of_scanline = true;
		m_fifo.screen_x = 0;
		m_fifo.fetcher_x = 0;
		m_fifo.discard = 0;

		//latch values of scx, scy and ly for next line
		latch_start_line_values();
	}
}

void PPU::draw_tick() {
	tick_bg_fetcher();
	output_pixels();

	if (m_fifo.screen_x >= 160) {
		m_ppu.ticks_in_hblank = SCANLINE_LENGTH - (m_ppu.ticks + OAM_DURATION);
		m_ppu.ticks = 0;

		m_ppu.current_mode = ppu_hblank;
	}
}

void PPU::hblank_tick() {
	if (m_ppu.ticks == m_ppu.ticks_in_hblank) {
		m_ppu.ticks = 0;
	
		//clear fifo
		for (int i = 0; i < background_fifo.size(); i++) {
			background_fifo.pop();
		}
		
		m_ppu.current_mode = (++m_ppu_io.ly >= DISPLAY_HEIGHT) ? ppu_vblank : ppu_oam;
		m_ppu.send_vblank = true;
		m_ppu.ticks_until_vblank = 4;

		if (m_ppu.current_mode == ppu_vblank) {
			m_ppu.is_frame_ready = true;
			m_draw_data->completed_frame = m_draw_data->frame;
		}
	}
}

void PPU::vblank_tick() {
	if (m_ppu.send_vblank) {
		m_ppu.ticks_until_vblank--;
		if (m_ppu.ticks_until_vblank == 0) {
			Interrupts::send_interrupt(interrupt_vblank);
			m_ppu.send_vblank = false;
		}
	}
	
	if (m_ppu.ticks == SCANLINE_LENGTH) {
		m_ppu.current_mode = (++m_ppu_io.ly >= DISPLAY_HEIGHT + 10) ? ppu_oam : ppu_vblank;
		m_ppu.ticks = 0;
	
		if (m_ppu.current_mode == ppu_oam) {
			m_ppu_io.ly = 0;
		}
	}
}

void PPU::check_ly_lyc() {
	if (m_ppu_io.ly == m_ppu_io.lyc) {
		m_ppu_io.stat |= (0x2);
		Interrupts::send_interrupt(interrupt_lcd);
	}
	else {
		m_ppu_io.stat &= ~(0x2);
	}
}

void PPU::latch_start_line_values() {
	m_ppu.current_ly = m_ppu_io.ly;
	m_ppu.current_scx = m_ppu_io.scx;
	m_ppu.current_scy = m_ppu_io.scy;
}

//FIFO BG/SPRITE
void PPU::tick_bg_fetcher() {
	m_fifo.ticks++;

	if (m_fifo.ticks < 2) {
		return;
	}

	switch (m_fifo.background.current_state) {
	case fifo_fetch_tile_number:
		fetcher_number();
		m_fifo.background.current_state = fifo_fetch_low;
		m_fifo.ticks = 0;

		break;

	case fifo_fetch_low:
		fetcher_low();
		m_fifo.background.current_state = fifo_fetch_high;
		m_fifo.ticks = 0;

		break;

	case fifo_fetch_high:
		fetcher_high();
		m_fifo.background.current_state = fifo_pushing;
		m_fifo.ticks = 0;

		if (m_fifo.dummy_fetch) {
			m_fifo.dummy_fetch = false;
			m_fifo.background.current_state = fifo_fetch_tile_number;
		}
			
		break;

	case fifo_pushing:
		fetcher_push();

		m_fifo.background.current_state = fifo_fetch_tile_number;
		m_fifo.ticks = 0;
			
		break;

	default:
		break;
	}
}

void PPU::fetcher_number() {
	u16 tile_map_base = 0x9800;
	if (m_ppu_io.lcdc & 0x08) {
		tile_map_base = 0x9c00;
	}

	tile_map_base += ((m_ppu.current_scx / 8) + m_fifo.fetcher_x) & 0x1f;
	tile_map_base += 32 * (((m_ppu.current_ly + m_ppu.current_scy) & 0xff) / 8);

	m_fifo.background.tile_index = m_memory->vram[u16(tile_map_base - 0x8000)];

	u16 tile_data_address = 0x9000;
	if ((m_ppu_io.lcdc & 0x10) != 0) {
		tile_data_address = 0x8000;
		tile_data_address += (m_fifo.background.tile_index * 16);
	}
	else {
		s8 index = s8(m_fifo.background.tile_index);
		tile_data_address += (index * 16);
	}

	tile_data_address += (2 * ((m_ppu.current_ly + m_ppu.current_scy) % 8));
	m_fifo.background.tile_address = tile_data_address;
}

void PPU::fetcher_low() {
	m_fifo.background.tile_low = m_memory->vram[u16(m_fifo.background.tile_address - 0x8000)];
}

void PPU::fetcher_high() {
	m_fifo.background.tile_high = m_memory->vram[u16((m_fifo.background.tile_address + 1) - 0x8000)];
}

void PPU::fetcher_push() {
	if (background_fifo.empty()) {
		for (int bit = 7; bit >= 0; bit--) {
			u8 low = (m_fifo.background.tile_low >> bit) & 0x01;
			u8 high = (m_fifo.background.tile_high >> bit) & 0x01;

			u8 colour = (high << 1) | low;

			s_pixel p = {
				.colour = colour,
				.palette = m_ppu_io.bgp,
				.sprite = false
			};

			if (background_fifo.size() < 16) {
				background_fifo.push(p);
			}
		}

		if (m_fifo.discard_fetch) {
			return;
		}

		m_fifo.fetcher_x++;
	}
}

void PPU::output_pixels() {
	if (m_fifo.start_of_scanline) {
		m_fifo.discard = (m_ppu.current_scx % 8);
		m_fifo.start_of_scanline = false;
	}

	if (m_fifo.discard != 0) {
		//fine x scrolling
		if (!background_fifo.empty()) {
			background_fifo.pop();
			m_fifo.discard--;
			return;
		}
	}

	if (background_fifo.empty()) {
		//fifo should be empty to push pixels
		return;
	}

	s_pixel pixel_to_output = background_fifo.front();
	background_fifo.pop();

	int shift = pixel_to_output.colour * 2;
	int p_colour = (pixel_to_output.palette >> shift) & 0x03;

	m_draw_data->frame[m_ppu.current_ly * 160 + m_fifo.screen_x] = sb_colours[p_colour];
	m_fifo.screen_x++;

	//hacky
	if (m_fifo.screen_x == 8 && m_fifo.discard_fetch) {
		m_fifo.discard_fetch = false;
		m_fifo.screen_x = 0;
	}
}