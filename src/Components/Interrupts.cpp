#include <Components/Interrupts.h>

void Interrupts::send_interrupt(e_interrupts type) {
	IF |= (0x01 << type);
}

void Interrupts::clear_interrupt(e_interrupts type) {
	IF &= ~(0x01 << type);
}

u8 Interrupts::read_if() {
	return IF;
}

void Interrupts::write_if(u8 value) {
	IF = value | 0xe0;
}

u8 Interrupts::read_ie() {
	return IE;
}

void Interrupts::write_ie(u8 value) {
	IE = value & 0x1f;
}

bool Interrupts::check_for_interrupt() {
	return currently_pending != interrupt_none;
}

e_interrupts Interrupts::get_new_pending() {
	currently_pending = interrupt_none;
	
	u8 interrupt = ((IF & IE) & 0x1f);
	for (int i = 0; i < 5; i++) {
		if (interrupt & (1 << i)) {
			currently_pending = (e_interrupts)i;
			break;
		}
	}

	return currently_pending;
}

e_interrupts Interrupts::get_current_pending() {
	return currently_pending;
}

u16 Interrupts::get_interrupt_vector(e_interrupts type) {
	switch (type) {
	case interrupt_vblank: return 0x0040;
	case interrupt_lcd: return 0x0048;
	case interrupt_timer: return 0x0050;
	case interrupt_serial: return 0x0058;
	case interrupt_joypad: return 0x0060;

	default:
		return 0x0000;
	}
}