#pragma once
#include "Utilities/Logger.h"
#include <array>

#include "bCPU.h"
#include "Bus.h"

class Syncroniser;

class CPU : public bCPU{
public:
	~CPU();

	//initialisation
	bool set_bus_ptr(Bus* bus);
	bool set_timing_ptr(Syncroniser* syncroniser);

private:
	Bus* m_bus = nullptr;
	Syncroniser* m_syncroniser = nullptr;

private:
	//VIRTUAL FUNCTIONS
	u8 read(u16 address) override;
	void write(u16 address, u8  value);
	u8 read_pc(bool read_interrupt = false) override;
	u16 read_pc_short() override;

	void tick_components(int ticks) override;
	void idle_cycle() override;
};