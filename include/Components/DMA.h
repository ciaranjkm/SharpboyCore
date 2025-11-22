#pragma once
#include "../Common.h"

const int DEFAULT_DMA_DELAY = 4;

struct s_dma_context {
	bool new_dma = false;
	int delay = 0;

	bool active = false;
	u8 start_byte = 0x00;

	u16 address = 0x0000;
	int cycles = 0;
	int ticks_this_cycle = 0;
};

class Bus;

class DMA {
public:
	~DMA();

	//TICK DMA
	void tick();
	
	bool start_dma(u8 start);
	void stop_dma();

	u8 get_dma_value() const;

	//SET PTRS FOR BUS
	void set_bus_ptr(Bus* bus);

private:
	Bus* m_bus = nullptr;
	s_dma_context m_dma = {};
};