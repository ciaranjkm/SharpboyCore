#include <Components/Timer.h>

//TICK
void Timer::tick() {
	if (m_tima_ticks_till_finish > 0) {
		m_tima_ticks_till_finish--;
		if (m_tima_ticks_till_finish == 0) {
			m_tima_reload_this_cycle = false;
		}
	}

	if (m_tima_ticks_till_irq > 0) {
		m_tima_ticks_till_irq--;
		if (m_tima_ticks_till_irq == 0) {
			m_timer_io.tima = m_timer_io.tma;

			Interrupts::send_interrupt(interrupt_timer);
			m_tima_ticks_till_finish = 4;
			m_tima_reload_this_cycle = true;
		}
	}

	update_sys_clock(m_timer_io.sys_clock + 1);
}

//RESET
void Timer::reset(bool using_boot_rom) {
	//todo using boot rom values init
	m_timer_io = {};
}

//IO ACCESS
u8 Timer::read_io(u16 address) {
	switch (address) {
	case io_div:
		return m_timer_io.sys_clock >> 8;

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
		update_sys_clock(0x0000);
		return;

	case io_tima:
		if (m_tima_ticks_till_irq > 0) {
			m_tima_reload_this_cycle = false;
			m_tima_ticks_till_irq = 0;
		}

		if (m_tima_ticks_till_finish > 0) {
			m_timer_io.tima = m_timer_io.tma;
			return;
		}

		m_timer_io.tima = value;
		return;

	case io_tma:
		if (m_tima_reload_this_cycle) {
			m_timer_io.tima = value;
		}

		m_timer_io.tma = value;
		return;

	case io_tac:
		handle_tac_write(value);
		return;

	default:
		return;
	}
}

//MEMBER FUNCTIONS
void Timer::update_sys_clock(u16 new_clock) {
	m_timer_io.sys_clock = new_clock;

	u8 div_bit = get_sys_clock_bit();
	u8 timer_enabled = is_tac_enabled();

	u8 current_timer_bit = div_bit & timer_enabled;
	detect_edge_case(previous_timer_bit, current_timer_bit);
	previous_timer_bit = current_timer_bit;
}

void Timer::handle_tac_write(u8 value) {
	u8 div_bit = get_sys_clock_bit();
	u8 timer_enabled = (value & 0x04) >> 2;

	u8 new_timer_bit = div_bit & timer_enabled;
	detect_edge_case(new_timer_bit, previous_timer_bit);
	previous_timer_bit = new_timer_bit;
	m_timer_io.tac = value;
}

u8 Timer::get_sys_clock_bit() {
	u8 tac_bit = m_timer_io.tac & 0x03;
	switch (tac_bit) {
	case 0x00:
		return (m_timer_io.sys_clock >> 9) & 0x01;

	case 0x01:
		return (m_timer_io.sys_clock >> 3) & 0x01;

	case 0x02:
		return (m_timer_io.sys_clock >> 5) & 0x01;

	case 0x03:
		return (m_timer_io.sys_clock >> 7) & 0x01;

	default:
		return (m_timer_io.sys_clock >> 9) & 0x01;
	}
}

u8 Timer::is_tac_enabled() {
	return ((m_timer_io.tac & 0x04) >> 2);
}

void Timer::detect_edge_case(u8 previous, u8 current) {
	if (previous && !current) {
		m_timer_io.tima += 1;
		if (m_timer_io.tima == 0x00) {
			start_tima_interrupt();
		}
	}
}

void Timer::start_tima_interrupt() {
	m_tima_ticks_till_irq = 4;
	m_timer_io.tima = 0x00;
}