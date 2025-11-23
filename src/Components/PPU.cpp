#include <Components/PPU.h>
#include <Components/Bus.h>


PPU::PPU() {
	m_vram.resize(VRAM_SIZE);
}

bool PPU::set_bus_ptr(Bus* bus) {
	m_bus = bus;

	if (m_bus) {
		return true;
	}
	
	return false;
}

void PPU::reset(bool using_boot_rom) {
	//todo using boot rom init values
	m_vram.clear();
	m_vram.resize(VRAM_SIZE);

	m_oam.fill(0x00);

	m_ppu_io = {};
}

void PPU::tick() {
	m_ppu_ticks++;

	if ((m_ppu_ticks % SCANLINE_LENGTH) == 0) {
		m_ppu_io.ly++;
	}

	if (m_ppu_ticks == VBLANK_DOT_TIME) {
		Interrupts::send_interrupt(interrupt_vblank);
	}

	if (m_ppu_ticks >= DOTS_PER_FRAME) {
		m_ppu_io.ly = 0;
		m_ppu_ticks -= DOTS_PER_FRAME;
	}
}

void PPU::dma_tick() {
	bool started_new_this_tick = false;

	if (m_dma.start_new) {
		m_dma.ticks_since_start++;

		//ALLIGN TO CPU CLOCK + 1 M XYX
		if (m_dma.ticks_since_start == DEFAULT_DMA_DELAY + 1) {
			m_bus->dma_active();

			if (!m_dma.active) {
				m_dma.active = true;
			}

			started_new_this_tick = true;
		}
	}

	if (m_dma.active) {
		m_dma.ticks_this_cycle++ ;
		if (m_dma.ticks_this_cycle == DEFAULT_DMA_DELAY) {
			
			m_dma.ticks_this_cycle = 0;
			m_dma.cycles_this_transfer++;

			u8 value = m_bus->unblocked_read(m_dma.dma_address++);

			m_oam[m_dma.cycles_this_transfer - 1] = value;
		}

		if (m_dma.cycles_this_transfer >= DEFAULT_DMA_CYCLES) {
			m_dma = {};
			m_bus->dma_inactive();
		}
	}

	if (started_new_this_tick) {
		m_dma.start_new = false;
		m_dma.ticks_this_cycle = 0;
		m_dma.cycles_this_transfer = 0;

		m_dma.dma_address = (m_dma.start_byte << 8) & 0xff00;
	}
}

u8 PPU::read(u16 address) {
	if(address >= 0x8000 && address < 0xa000)  {
		return m_vram[(u16)(address - 0x8000)];
	}
	else if (address >= 0xfe00 && address < 0xfea0) {
		return m_oam[(u16)(address - 0xfe00)];
	}
	else {
		return 0xff;
	}
}

void PPU::write(u16 address, u8 value) {
	if (address >= 0x8000 && address < 0xa000) {
		m_vram[(u16)(address - 0x8000)] = value;
		return;
	}
	else if (address >= 0xfe00 && address < 0xfea0) {
		m_oam[(u16)(address - 0xfe00)] = value;
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

		printf("new dma 0x%02X\n", m_dma.start_byte);
		return;


	default:
		return ;
	}

}

u8 PPU::bus_read(u16 address) {
	//todo change to unblocked read
	return m_bus->cpu_read(address);
}