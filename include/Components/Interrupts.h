#pragma once

#include "InterruptDefs.h"
#include "../Common.h"

/*
	Interrupt manager for Sharpboy, using it as static to allow multiple components to access the same IF and IE.
	TOOO maybe make this not static to allow multiple instances of SharpboyCore
*/

class Interrupts {
public:
	static void send_interrupt(e_interrupts type);
	static void clear_interrupt(e_interrupts type);

	static u8 read_if();
	static void write_if(u8 value);

	static u8 read_ie();
	static void write_ie(u8 value);

	static bool check_for_interrupt();
	static e_interrupts get_new_pending();
	static e_interrupts get_current_pending();

	static u16 get_interrupt_vector(e_interrupts type);

private:
	static u8 IF;
	static u8 IE;

	static e_interrupts currently_pending;
};

inline u8 Interrupts::IF = 0x00;
inline u8 Interrupts::IE = 0xe0;

inline e_interrupts Interrupts::currently_pending = interrupt_none;