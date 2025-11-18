#pragma once
#include "Utilities/Logger.h"
#include "../../Common.h"
#include <array>

#include "bCPU.h"
#include "../Bus.h"
#include "../Interrupts.h"

class Syncroniser;

class CPU : public bCPU{
public:
	~CPU();

	//VIRTUAL FUNCTIONS
	int step() override;

	//initialisation
	bool set_bus_ptr(Bus* bus);
	bool set_sync_ptr(Syncroniser* syncroniser);

	int handle_interrupt();
	void check_enable_ime();
	void check_for_interrupt();

private:
	Bus* m_bus = nullptr;
	Syncroniser* m_syncroniser = nullptr;

private:
	//VIRTUAL FUNCTIONS
	u8 read(u16 address) override;
	void write(u16 address, u8  value) override;

	u8 read_pc(bool read_interrupt = false) override;
	u16 read_pc_short() override;

	void tick_components(int ticks) override;
	void idle_cycle() override;

	//MEMBER FUNCTIONS
	u8 nontick_read(u16 address);
	void nontick_write(u16 address, u8 value);

	e_interrupts get_interrupt_pending();
	void clear_if_bit(e_interrupts bit_to_clear);
};