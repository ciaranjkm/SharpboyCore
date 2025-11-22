#include <Components/DMA.h>
#include <Components/Bus.h>

#include <iostream>

DMA::~DMA() {
	m_bus = nullptr;
}

//TICK DMA
void DMA::tick() {
	if (m_dma.new_dma) {
		m_dma.delay--;

		if (m_dma.delay == 0) {
			m_dma.active = true;
			m_dma.new_dma = false;

			m_dma.address = m_dma.start_byte << 8;
			m_dma.cycles = 0;
			m_dma.ticks_this_cycle = 0;
			return;
		}
	}

	if (m_dma.active) {
		m_dma.ticks_this_cycle++;
		//printf("ticks: %d\n", m_dma.ticks_this_cycle);

		if (m_dma.ticks_this_cycle == DEFAULT_DMA_DELAY) {
			m_dma.ticks_this_cycle -= DEFAULT_DMA_DELAY;

			m_dma.cycles++;

			u8 value = m_bus->dma_read(m_dma.address);
			m_dma.address++;

			m_bus->dma_write(m_dma.cycles - 1, value);
		}

		if (m_dma.cycles == 160) {
			m_dma.active = false;
			stop_dma();
		}
	}
}

bool DMA::start_dma(u8 start) {
	m_dma.new_dma = true;
	m_dma.delay = DEFAULT_DMA_DELAY;
	m_dma.start_byte = start;

	//TODO sources_GS test with external RAM failing that test at 9f read check
	//printf("start byte: 0x%02X", m_dma.start_byte);

	return true;
}

void DMA::stop_dma() {
	m_bus->stop_dma_active();
}

u8 DMA::get_dma_value() const {
	return m_dma.start_byte;
}

void DMA::set_bus_ptr(Bus* bus) {
	m_bus = bus;
}