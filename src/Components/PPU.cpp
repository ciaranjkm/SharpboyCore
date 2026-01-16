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
	if (m_ppu.ly_increment) {
		m_ppu.ly_increment = false;
		m_ppu_io.ly = m_ppu.new_ly;

		//check_ly_lyc();
	}

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
	return m_ppu.is_frame_ready;
}

void PPU::reset_frame_ready() {
	m_ppu.is_frame_ready = false;
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
void PPU::oam_tick() {
	if (m_ppu.ticks == OAM_DURATION) {
		m_ppu.ticks = 0;
		m_ppu.current_mode = ppu_draw;

		change_stat_mode(m_ppu.current_mode);

		latch_start_line_values();

		reset_fifos();
	}
}

void PPU::draw_tick() {
	tick_bg_fetcher();
	output_pixels();

	if (m_fifo.screen_x >= DISPLAY_WIDTH) {
		m_ppu.ticks_in_hblank = SCANLINE_LENGTH - (m_ppu.ticks + OAM_DURATION);
		m_ppu.ticks = 0;

		m_ppu.current_mode = ppu_hblank;

		change_stat_mode(m_ppu.current_mode);
	}
}

void PPU::hblank_tick() {
	if (m_ppu.ticks == m_ppu.ticks_in_hblank) {
		m_ppu.ticks = 0;
		
		m_ppu.new_ly = u8(m_ppu_io.ly + 1);
		m_ppu.current_mode = (m_ppu.new_ly >= DISPLAY_HEIGHT) ? ppu_vblank : ppu_oam;
		
		change_stat_mode(m_ppu.current_mode);

		m_ppu.ly_increment = true;

		if (m_ppu.current_mode == ppu_vblank) {
			m_ppu.is_frame_ready = true;
			m_draw_data->completed_frame = m_draw_data->frame;

			m_ppu.send_vblank = true;
			m_ppu.ticks_until_vblank = 4;
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
		m_ppu.new_ly = u8(m_ppu_io.ly + 1);
		m_ppu.current_mode = (m_ppu.new_ly >= DISPLAY_HEIGHT + 10) ? ppu_oam : ppu_vblank;
		m_ppu.ticks = 0;

		m_ppu.ly_increment = true;

		if (m_ppu.current_mode == ppu_oam) {
			change_stat_mode(m_ppu.current_mode);

			m_ppu.new_ly = 0x00;
		}
	}
}

//INTERRUPTS + STAT

void PPU::check_ly_lyc() {
	bool previous = ((m_ppu_io.stat & 0x02) != 0x00);
	bool new_irq = m_ppu_io.ly == m_ppu_io.lyc;

	if (new_irq) {
		m_ppu_io.stat |= (0x2);
	}
	else {
		m_ppu_io.stat &= ~(0x2);
	}

	if (!previous && new_irq) {
		if ((m_ppu_io.stat & 0x40) != 0x00) {
			Interrupts::send_interrupt(interrupt_lcd);
		}
	}
}

void PPU::latch_start_line_values() {
	m_ppu_io.ly = m_ppu_io.ly;
	m_ppu.current_scx = m_ppu_io.scx;
	m_ppu_io.scy = m_ppu_io.scy;
}

void PPU::change_stat_mode(e_ppu_mode new_mode) {
	u8 mode_value = 0x00;
	
	switch (new_mode) {
	case ppu_hblank:
		mode_value = 0x00;
		break;

	case ppu_vblank:
		mode_value = 0x01;
		break;

	case ppu_oam:
		mode_value = 0x02;
		break;

	case ppu_draw:
		mode_value = 0x03;
		break;

	default:
		return;
	}

	m_ppu_io.stat = (m_ppu_io.stat & ~0x03) | mode_value;

	if (new_mode == ppu_draw) {
		return;
	}

	if ((m_ppu_io.stat & (1 << (mode_value + 0x03)))) {
		Interrupts::send_interrupt(interrupt_lcd);
	}
}

//FIFO BG/SPRITE
void PPU::tick_bg_fetcher() {
	m_fifo.ticks++;

	if (m_fifo.ticks < 2) {
		return;
	}

	m_fifo.ticks = 0;

	switch (m_fifo.bg_w.current_state) {
	case fifo_fetch_tile_number:
		fetcher_number();
		m_fifo.bg_w.current_state = fifo_fetch_low;
		break;

	case fifo_fetch_low:
		fetcher_low();
		m_fifo.bg_w.current_state = fifo_fetch_high;
		break;

	case fifo_fetch_high:
		fetcher_high();
		m_fifo.bg_w.current_state = fifo_pushing;
		
		if (m_fifo.start_of_scanline) {
			m_fifo.start_of_scanline = false;
			m_fifo.bg_w.current_state = fifo_fetch_tile_number;
		}

		break;

	case fifo_pushing:
		fetcher_push();
		m_fifo.bg_w.current_state = fifo_fetch_tile_number;
		break;

	default:
		return;
	}

}

void PPU::fetcher_number() {
	u16 tile_map_base = 0x9800;
	if ((m_ppu_io.lcdc & 0x08) != 0x00) {
		tile_map_base = 0x9c00;
	}

	tile_map_base += ((m_ppu_io.scx / 8) + m_fifo.bg_w.fetcher_x);
	tile_map_base += 32 * (((m_ppu_io.ly + m_ppu_io.scy) & 0xff) / 8);

	m_fifo.bg_w.tile_index = m_memory->vram[u16(tile_map_base - 0x8000)];

	u16 tile_data_address = 0x9000;
	if (m_ppu_io.lcdc & 0x10) {
		tile_data_address = 0x8000;
		tile_data_address += (m_fifo.bg_w.tile_index * 16);
	}
	else {
		if (m_fifo.bg_w.tile_index > 0x7f) {
			tile_data_address = 0x8800;
		}
		else {
			tile_data_address = 0x9000;
		}
	}

	tile_data_address += (2 * ((m_ppu_io.ly + m_ppu_io.scy) % 8));
	m_fifo.bg_w.tile_address = tile_data_address;
}

void PPU::fetcher_low() {
	m_fifo.bg_w.tile_low = m_memory->vram[u16(m_fifo.bg_w.tile_address - 0x8000)];
}

void PPU::fetcher_high() {
	m_fifo.bg_w.tile_high = m_memory->vram[u16((m_fifo.bg_w.tile_address + 1) - 0x8000)];
}

void PPU::fetcher_push() {
	if (background_fifo.size() < 8) {
		for (int b = 7; b > 0; b--) {
			u8 low = (m_fifo.bg_w.tile_low >> b) & 0x01;
			u8 high = (m_fifo.bg_w.tile_high >> b) & 0x01;

			u8 colour = (high << 1) | low;

			s_pixel p = {
				.colour = colour,
				.palette = m_ppu_io.bgp,
				.sprite = false
			};

			background_fifo.push(p);
		}

		m_fifo.bg_w.fetcher_x++;
	}
}

void PPU::reset_fifos() {
	m_fifo = {};

	while (!background_fifo.empty()) {
		background_fifo.pop();
	}
}

u16 PPU::convert_tile_id_to_address(u8 tile_id) {
	if (m_ppu_io.lcdc & 0x10) {
		u16 tile_data_address = 0x8000 + (tile_id * 0x10);
		tile_data_address += ((2 * (m_ppu_io.ly + m_ppu_io.scy)) % 8);
		return tile_data_address;
	}
	else {
		s8 s_tile_id = s8(tile_id);
		u16 tile_data_address = 0x9000 + (s_tile_id * 0x10);
		tile_data_address += ((2 * (m_ppu_io.ly + m_ppu_io.scy)) % 8);
		return tile_data_address;
	}
}

void PPU::output_pixels() {
	if (!background_fifo.empty()) {
		if (!m_fifo.ready) {
			if (background_fifo.size() >= 8) {
				for (int i = 0; i < (m_ppu_io.scx % 0x08); i++) {
					background_fifo.pop();
				}

				m_fifo.ready = true;
				return;
			}
		}

		if (m_fifo.ready) {
			s_pixel out = background_fifo.front();
			background_fifo.pop();

			int shift = out.colour * 2;
			int colour = (m_ppu_io.bgp >> shift) & 0x03;

			m_draw_data->frame[m_ppu_io.ly * DISPLAY_WIDTH + m_fifo.screen_x] = sb_colours[colour];
			m_fifo.screen_x++;
		}
	}
}