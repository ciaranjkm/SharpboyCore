#include <Interrupts.h>

void Interrupts::send_interrupt(e_interrupts type) {
	IF |= (0x01 << type);
}

void Interrupts::clear_interrupt(e_interrupts type) {
	IF &= ~(0x01 << type);
}

u8 Interrupts::read_if() {
	return IF & 0x1f;
}

void Interrupts::write_if(u8 value) {
	IF = value;
}

u8 Interrupts::read_ie() {
	return IE;
}

void Interrupts::write_ie(u8 value) {
	IE = value;
}

bool Interrupts::check_for_interrupt() {
	return ((IF & IE) & 0x1f) != 0;
}

e_interrupts Interrupts::get_pending_interrupt() {
	int bit = -1;
	u8 interrupt = ((IF & IE) & 0x1f);

	for (int i = 0; i < 5; i++) {
		if (interrupt & (1 << i)) {
			bit = i;
			break;
		}

		i = -1;
	}

	return (e_interrupts)bit;
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