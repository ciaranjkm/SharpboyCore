#pragma once

#include "InterruptDefs.h"
#include "../Common.h"

class Interrupts {
public:
	static void send_interrupt(e_interrupts type);
	static void clear_interrupt(e_interrupts type);

	static u8 read_if();
	static void write_if(u8 value);

	static u8 read_ie();
	static void write_ie(u8 value);

	static bool check_for_interrupt();
	static e_interrupts get_pending_interrupt();
	static u16 get_interrupt_vector(e_interrupts type);

private:
	static u8 IF;
	static u8 IE;
};

inline u8 Interrupts::IF = 0x00;
inline u8 Interrupts::IE = 0x00;