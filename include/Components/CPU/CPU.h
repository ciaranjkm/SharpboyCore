#pragma once
#include "Utilities/SBLogger.h"
#include "../../Common.h"
#include <array>

#include "bCPU.h"
#include "../Bus.h"
#include "../Interrupts.h"

class Syncroniser;

class CPU final : public bCPU {
public:
	~CPU();

	//VIRTUAL FUNCTIONS
	int step() override;

	//initialisation
	bool set_bus_ptr(Bus* bus);
	bool set_sync_ptr(Syncroniser* syncroniser);

private:
	Bus* m_bus = nullptr;
	Syncroniser* m_syncroniser = nullptr;

	e_interrupts interrupt_pending = interrupt_none;

private:
	//VIRTUAL FUNCTIONS
	u8 read(u16 address) override;
	void write(u16 address, u8  value) override;

	u8 read_pc(bool read_interrupt = false) override;
	u16 read_pc_short() override;

	void tick_components(int ticks) override;
	void idle_cycle() override;

	//MEMBER FUNCTIONS
	int handle_interrupt();
	void check_halt_bug();
	void check_enable_ime();
};
