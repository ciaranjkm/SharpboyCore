#pragma once
#include "Utilities/Common.h"
#include "Utilities/Testing.h"
#include "Utilities/Logger.h"
#include <array>

#include "Bus.h"
#include "InstrDefs.h"

struct s_registers {
	u8 a, f, b, c, d, e, h, l;
	u16 pc, sp;
};

enum e_flags {
	NO_FLAG = -1,
	fZERO = 7,
	fSUB = 6,
	fHCARRY = 5,
	fCARRY = 4
};

enum e_joined_regs {
	rAF,
	rBC,
	rDE,
	rHL
};

class Timing;

class CPU {
public:
	~CPU();

	//execution
	int execute_next_instruction();

	//initialisation
	bool set_bus_ptr(Bus* bus);
	bool set_timing_ptr(Timing* timing);

	void reset_regs();
	void reset_regs(std::array<u8 , 0x08> regs, u16 sp, u16 pc);

	//sst
	bool start_sst_mode(SST_Tester* sst_tester);
	void reset_sst();

	//debug
	s_registers* get_registers();

private:
	s_registers m_registers;
	Bus* m_bus = nullptr;
	Timing* m_timing = nullptr;

	u8 interrupt_pending = 0x00;
	bool halt_bug = false;
	bool halted = false;

	bool ime = false;
	bool enable_ime = false;

	//TESTING PTR
	bool sst = false;
	SST_Tester* tester = nullptr;

private:
	//execution
	int execute_opcode(u8  opcode);
	int execute_cb_opcode(int cycles);

	//timing
	void tick_components(int ticks);
	void idle_cycle();

	//memory access
	u8  read(u16 address);
	void write(u16 address, u8  value);

	u8 read_pc(bool read_interrupt = false);
	u16 read_pc_short();

	//flag helpers 
	bool get_flag(e_flags flag) const;
	void set_flag(e_flags flag, bool state);

	//register helpers
	u16 get_joined_reg(e_joined_regs reg) const;
	void set_joined_reg(e_joined_regs reg, u16 value);

	//sst
	void add_test_cycle(u16 address, u8  value, std::string operation);

private:
	//opcode functions

	//8 bit load instructions
	int ld_r_r(u8 & dest, u8 src);
	int ld_r_n(u8 & dest);
	int ld_r_hl(u8 & dest);
	int ld_hl_r(u8 src);
	int ld_hl_n();
	int ld_a_bc();
	int ld_a_de();
	int ld_bc_a();
	int ld_de_a();
	int ld_a_nn();
	int ld_nn_a();
	int ldh_a_c();
	int ldh_c_a();
	int ldh_a_n();
	int ldh_n_a();
	int ld_a_hl_dec();
	int ld_hl_dec_a();
	int ld_a_hl_inc();
	int ld_hl_inc_a();

	//16 bit load instructions
	int ld_rr_nn(e_joined_regs dest);
	int ld_rr_nn(u16& dest);
	int ld_nn_sp();
	int ld_sp_hl();
	int push_rr(e_joined_regs src);
	int pop_rr(e_joined_regs dest);
	int ld_hl_sp_offset();

	//8 bit arithmetic and logic instructions
	int add_r(u8 src);
	int add_hl();
	int add_n();
	int adc_r(u8 src);
	int adc_hl();
	int adc_n();
	int sub_r(u8 src);
	int sub_hl();
	int sub_n();
	int sbc_r(u8 src);
	int sbc_hl();
	int sbc_n();
	int cp_r(u8 src);
	int cp_hl();
	int cp_n();
	int inc_r(u8 & src);
	int inc_hl();
	int dec_r(u8 & src);
	int dec_hl();
	int and_r(u8 src);
	int and_hl();
	int and_n();
	int or_r(u8 src);
	int or_hl();
	int or_n();
	int xor_r(u8 src);
	int xor_hl();
	int xor_n();
	int ccf();
	int scf();
	int daa();
	int cpl();

	//16 bit arithmetic
	int inc_rr(e_joined_regs src);
	int dec_rr(e_joined_regs src);
	int add_hl_rr(e_joined_regs src);
	int add_hl_rr(u16 src);
	int add_sp_e();

	//rotate shift and bit op instructions
	int rlca();
	int rrca();
	int rla();
	int rra();
	int rlc_r(u8 & src);
	int rlc_hl();
	int rrc_r(u8 & src);
	int rrc_hl();
	int rl_r(u8 & src);
	int rl_hl();
	int rr_r(u8 & src);
	int rr_hl();
	int sla_r(u8 & src);
	int sla_hl();
	int sra_r(u8 & src);
	int sra_hl();
	int swap_r(u8 & src);
	int swap_hl();
	int srl_r(u8 & src);
	int srl_hl();
	int bit_b_r(int bit, u8 src);
	int bit_b_hl(int bit);
	int res_b_r(int bit, u8 & src);
	int res_b_hl(int bit);
	int set_b_r(int bit, u8 & src);
	int set_b_hl(int bit);

	//control flow instructions
	int jp_nn();
	int jp_hl();
	int jp_cc(bool condition);
	int jr_e();
	int jr_cc(bool condition);
	int call_nn();
	int call_cc(bool condition);
	int ret();
	int ret_cc(bool condition);
	int reti();
	int rst_n(u8 vector);

	//misc instructions
	int halt();
	int stop();
	int ei();
	int di();
};