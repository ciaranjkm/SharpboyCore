#include <Components/Timer.h>
#include <Components/CPU/CPU.h>

//TICK
void Timer::tick(CPU* m_cpu) {
	m_timer_io.div++;

	int tac_bit = get_tac_bit();

	bool timer_enabled = is_timer_enabled();
	bool div_bit_selected = (m_timer_io.div & (1 << tac_bit)) != 0x00;

	bool and_result = div_bit_selected && timer_enabled;
	if (previous_and_result && !and_result) {
		m_timer_io.tima++;
		if (m_timer_io.tima == 0x00) {
			reload_tima = true;
			m_timer_io.tima = 0x00;
			tima_delay = DEFAULT_TIMA_DELAY;
		}
	}

	previous_and_result = and_result;

	if (reload_tima) {
		tima_delay--;

		if (tima_delay == 4) {
			//AFTER ONE M CYCLE LOAD TMA INTO TMA
			m_timer_io.tima = m_timer_io.tma;
		}

		if (tima_delay == 0) {
			reload_tima = false;
			tima_delay = 0;

			//LOAD TIMA AGAIN WITH TMA INCASE OF A WRITE AND THEN SEND INTERRUPT
			m_timer_io.tima = m_timer_io.tma;
			Interrupts::send_interrupt(interrupt_timer);
		}
	}
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
		if (reload_tima) {
			if (tima_delay < DEFAULT_TIMA_DELAY && tima_delay >= DEFAULT_TIMA_DELAY / 2) {
				//CANCEL TIMA RELOAD ON M CYCLE ONE
				reload_tima = false;
				m_timer_io.tima = value;
				return;
			}
			else if (tima_delay <= DEFAULT_TIMA_DELAY / 2 && tima_delay >= 0) {
				//DONT DO ANYTHING IF ON M CYCLE TWO
				return;
			}
		}

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

int Timer::get_tac_bit() {
	switch (m_timer_io.tac & 0x03) {
	case 0x00: 
		return 9; //default 4096

	case 0x01: 
		return 3; 

	case 0x02: 
		return 5; 

	case 0x03: 
		return 7;

	default:
		return 9;
	}
}

bool Timer::is_timer_enabled() {
	return (m_timer_io.tac & 0x4) != 0x00;
}