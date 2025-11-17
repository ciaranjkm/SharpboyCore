#include <Timer.h>

//TICK
void Timer::tick() {

}

//IO ACCESS
u8 Timer::read_io(u16 address) {
	switch (address) {
	case io_div:
		return m_timer_io.div & 0xff;

	case io_tima:
		return m_timer_io.tima;

	case io_tma:
		return m_timer_io.tma;

	case io_tac:
		return m_timer_io.tac;
	
	default:
		return 0xff;	
	}
}

void Timer::write_io(u16 address, u8 value) {
	switch (address) {
	case io_div:
		m_timer_io.div = 0x0000;
		return;

	case io_tima:
		m_timer_io.tima = value;
		return;

	case io_tma:
		m_timer_io.tma = value;
		return;

	case io_tac:
		m_timer_io.tac = value;
		return;

	default:
		return;
	}
}