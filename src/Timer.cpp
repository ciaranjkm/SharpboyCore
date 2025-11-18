#include <Components/Timer.h>
#include <Components/CPU/CPU.h>

//TICK
void Timer::tick(CPU* m_cpu) {
	m_timer_io.div++;
	/*

	//select timer bit from tac
	int bit_selected = 9;
	switch (m_timer_io.tac & 0x03) {
	case 0b00: bit_selected = 9; // 4096 Hz
	case 0b01: bit_selected = 3; // 262144 Hz
	case 0b10: bit_selected = 5; // 65536 Hz
	case 0b11: bit_selected = 7; // 16384 Hz
	}
	bool div_bit_selected = (m_timer_io.div & (1 << bit_selected)) != 0x00;
	bool timer_enabled = (m_timer_io.tac & 0x4) != 0x00;

	//find falling edge result
	bool and_result = div_bit_selected && timer_enabled;

	//inc timer if edge case
	if (previous_and_result && !and_result) {
		m_timer_io.tima++;
		if (m_timer_io.tima == 0x00) {
			reload_tima = true;
			tima_delay = DEFAULT_TIMA_DELAY;
		}

		previous_and_result = and_result;
	}

	if (reload_tima) {
		tima_delay--;

		//after 1 m cycle load tma into tma
		if (tima_delay == 4) {
			m_timer_io.tima = m_timer_io.tma;
		}

		//complete reload of tima and trigger interrupt, load with new tma incase of a new write on t cycle 2 of m cycle 2
		if (tima_delay == 2) {
			m_timer_io.tima = m_timer_io.tma;
			
			u8 IF = m_cpu->read(io_if);
			IF |= (1 << 2);
			m_cpu->write(io_if, IF);
		}

		//when tima delay is complete turn off tima reload
		if (tima_delay == 0) {
			reload_tima = false;
			tima_delay = -1;
		}
	}

	previous_and_result = and_result;
	*/
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
		return m_timer_io.div >> 8;

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