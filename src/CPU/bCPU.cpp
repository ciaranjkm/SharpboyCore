#include <CPU/bCPU.h>

//BASE CPU FUNCTIONS FOR OPERATION

int bCPU::execute_next_instruction() {
	int cycles = 0;
	u8 opcode = read_pc();

	cycles = execute_opcode(opcode);
	return cycles;
}

//todo make reset based on boot rom or not
void bCPU::reset(bool using_boot_rom) {
	if (!using_boot_rom) {
		m_registers = {};
		return;
	}

	m_registers = POST_BOOT_ROM_REGS;
}

s_registers* bCPU::get_registers() {
	return &m_registers;
}

int bCPU::execute_opcode(u8 opcode) {
	int cycles = 0;

	switch (opcode) {
		// 0x00 -> 0x0f
	case inst_NOOP: break;
	case inst_LD_BC_NN:		 cycles = ld_rr_nn(rBC); break;
	case inst_LD_BC_A:		 cycles = ld_bc_a(); break;
	case inst_INC_BC:		 cycles = inc_rr(rBC); break;
	case inst_INC_B:		 cycles = inc_r(m_registers.b); break;
	case inst_DEC_B:		 cycles = dec_r(m_registers.b); break;
	case inst_LD_B_N:		 cycles = ld_r_n(m_registers.b); break;
	case inst_RLCA:			 cycles = rlca(); break;
	case inst_LD_NN_SP:		 cycles = ld_nn_sp(); break;
	case inst_ADD_HL_BC:	 cycles = add_hl_rr(rBC); break;
	case inst_LD_A_BC:		 cycles = ld_a_bc(); break;
	case inst_DEC_BC:		 cycles = dec_rr(rBC); break;
	case inst_INC_C:		 cycles = inc_r(m_registers.c); break;
	case inst_DEC_C:		 cycles = dec_r(m_registers.c); break;
	case inst_LD_C_N:		 cycles = ld_r_n(m_registers.c); break;
	case inst_RRCA:			 cycles = rrca(); break;

		// 0x10 -> 0x1f
	case inst_STOP_N:        cycles = stop(); break;
	case inst_LD_DE_NN:      cycles = ld_rr_nn(rDE); break;
	case inst_LD_DE_A:       cycles = ld_de_a(); break;
	case inst_INC_DE:        cycles = inc_rr(rDE); break;
	case inst_INC_D:         cycles = inc_r(m_registers.d); break;
	case inst_DEC_D:         cycles = dec_r(m_registers.d); break;
	case inst_LD_D_N:        cycles = ld_r_n(m_registers.d); break;
	case inst_RLA:           cycles = rla(); break;
	case inst_JR_E:          cycles = jr_e(); break;
	case inst_ADD_HL_DE:     cycles = add_hl_rr(rDE); break;
	case inst_LD_A_DE:       cycles = ld_a_de(); break;
	case inst_DEC_DE:        cycles = dec_rr(rDE); break;
	case inst_INC_E:         cycles = inc_r(m_registers.e); break;
	case inst_DEC_E:         cycles = dec_r(m_registers.e); break;
	case inst_LD_E_N:        cycles = ld_r_n(m_registers.e); break;
	case inst_RRA:           cycles = rra(); break;

		// 0x20 -> 0x2f
	case inst_JR_NZ_E:       cycles = jr_cc(!get_flag(fZERO)); break;
	case inst_LD_HL_NN:      cycles = ld_rr_nn(rHL); break;
	case inst_LDI_HL_A:      cycles = ld_hl_inc_a(); break;
	case inst_INC_HL:        cycles = inc_rr(rHL); break;
	case inst_INC_H:         cycles = inc_r(m_registers.h); break;
	case inst_DEC_H:         cycles = dec_r(m_registers.h); break;
	case inst_LD_H_N:        cycles = ld_r_n(m_registers.h); break;
	case inst_DAA:           cycles = daa(); break;
	case inst_JR_Z_E:        cycles = jr_cc(get_flag(fZERO)); break;
	case inst_ADD_HL_HL:     cycles = add_hl_rr(rHL); break;
	case inst_LD_A_HLI:      cycles = ld_a_hl_inc(); break;
	case inst_DEC_HL:        cycles = dec_rr(rHL); break;
	case inst_INC_L:         cycles = inc_r(m_registers.l); break;
	case inst_DEC_L:         cycles = dec_r(m_registers.l); break;
	case inst_LD_L_N:        cycles = ld_r_n(m_registers.l); break;
	case inst_CPL:           cycles = cpl(); break;

		// 0x30 -> 0x3f
	case inst_JR_NC_E:       cycles = jr_cc(!get_flag(fCARRY)); break;
	case inst_LD_SP_NN:      cycles = ld_rr_nn(m_registers.sp); break;
	case inst_LDD_HL_A:      cycles = ld_hl_dec_a(); break;
	case inst_INC_SP:        cycles = ticks_4; idle_cycle(); m_registers.sp++;  break;
	case inst_INC_memHL:     cycles = inc_hl(); break;
	case inst_DEC_memHL:     cycles = dec_hl(); break;
	case inst_LD_HL_N:       cycles = ld_hl_n(); break;
	case inst_SCF:           cycles = scf(); break;
	case inst_JR_C_E:        cycles = jr_cc(get_flag(fCARRY)); break;
	case inst_ADD_HL_SP:     cycles = add_hl_rr(m_registers.sp); break;
	case inst_LD_A_HLD:      cycles = ld_a_hl_dec(); break;
	case inst_DEC_SP:        cycles = ticks_4; idle_cycle(); m_registers.sp--;  break;
	case inst_INC_A:         cycles = inc_r(m_registers.a); break;
	case inst_DEC_A:         cycles = dec_r(m_registers.a); break;
	case inst_LD_A_N:        cycles = ld_r_n(m_registers.a); break;
	case inst_CCF:           cycles = ccf(); break;

		// 0x40 -> 0x4f
	case inst_LD_B_B:        cycles = ld_r_r(m_registers.b, m_registers.b); break;
	case inst_LD_B_C:        cycles = ld_r_r(m_registers.b, m_registers.c); break;
	case inst_LD_B_D:        cycles = ld_r_r(m_registers.b, m_registers.d); break;
	case inst_LD_B_E:        cycles = ld_r_r(m_registers.b, m_registers.e); break;
	case inst_LD_B_H:        cycles = ld_r_r(m_registers.b, m_registers.h); break;
	case inst_LD_B_L:        cycles = ld_r_r(m_registers.b, m_registers.l); break;
	case inst_LD_B_HL:       cycles = ld_r_hl(m_registers.b); break;
	case inst_LD_B_A:        cycles = ld_r_r(m_registers.b, m_registers.a); break;
	case inst_LD_C_B:        cycles = ld_r_r(m_registers.c, m_registers.b); break;
	case inst_LD_C_C:        cycles = ld_r_r(m_registers.c, m_registers.c); break;
	case inst_LD_C_D:        cycles = ld_r_r(m_registers.c, m_registers.d); break;
	case inst_LD_C_E:        cycles = ld_r_r(m_registers.c, m_registers.e); break;
	case inst_LD_C_H:        cycles = ld_r_r(m_registers.c, m_registers.h); break;
	case inst_LD_C_L:        cycles = ld_r_r(m_registers.c, m_registers.l); break;
	case inst_LD_C_HL:       cycles = ld_r_hl(m_registers.c); break;
	case inst_LD_C_A:        cycles = ld_r_r(m_registers.c, m_registers.a); break;

		// 0x50 -> 0x5f
	case inst_LD_D_B:        cycles = ld_r_r(m_registers.d, m_registers.b); break;
	case inst_LD_D_C:        cycles = ld_r_r(m_registers.d, m_registers.c); break;
	case inst_LD_D_D:        cycles = ld_r_r(m_registers.d, m_registers.d); break;
	case inst_LD_D_E:        cycles = ld_r_r(m_registers.d, m_registers.e); break;
	case inst_LD_D_H:        cycles = ld_r_r(m_registers.d, m_registers.h); break;
	case inst_LD_D_L:        cycles = ld_r_r(m_registers.d, m_registers.l); break;
	case inst_LD_D_HL:       cycles = ld_r_hl(m_registers.d); break;
	case inst_LD_D_A:        cycles = ld_r_r(m_registers.d, m_registers.a); break;
	case inst_LD_E_B:        cycles = ld_r_r(m_registers.e, m_registers.b); break;
	case inst_LD_E_C:        cycles = ld_r_r(m_registers.e, m_registers.c); break;
	case inst_LD_E_D:        cycles = ld_r_r(m_registers.e, m_registers.d); break;
	case inst_LD_E_E:        cycles = ld_r_r(m_registers.e, m_registers.e); break;
	case inst_LD_E_H:        cycles = ld_r_r(m_registers.e, m_registers.h); break;
	case inst_LD_E_L:        cycles = ld_r_r(m_registers.e, m_registers.l); break;
	case inst_LD_E_HL:       cycles = ld_r_hl(m_registers.e); break;
	case inst_LD_E_A:        cycles = ld_r_r(m_registers.e, m_registers.a); break;

		// 0x60 -> 0x6f
	case inst_LD_H_B:        cycles = ld_r_r(m_registers.h, m_registers.b); break;
	case inst_LD_H_C:        cycles = ld_r_r(m_registers.h, m_registers.c); break;
	case inst_LD_H_D:        cycles = ld_r_r(m_registers.h, m_registers.d); break;
	case inst_LD_H_E:        cycles = ld_r_r(m_registers.h, m_registers.e); break;
	case inst_LD_H_H:        cycles = ld_r_r(m_registers.h, m_registers.h); break;
	case inst_LD_H_L:        cycles = ld_r_r(m_registers.h, m_registers.l); break;
	case inst_LD_H_HL:       cycles = ld_r_hl(m_registers.h); break;
	case inst_LD_H_A:        cycles = ld_r_r(m_registers.h, m_registers.a); break;
	case inst_LD_L_B:        cycles = ld_r_r(m_registers.l, m_registers.b); break;
	case inst_LD_L_C:        cycles = ld_r_r(m_registers.l, m_registers.c); break;
	case inst_LD_L_D:        cycles = ld_r_r(m_registers.l, m_registers.d); break;
	case inst_LD_L_E:        cycles = ld_r_r(m_registers.l, m_registers.e); break;
	case inst_LD_L_H:        cycles = ld_r_r(m_registers.l, m_registers.h); break;
	case inst_LD_L_L:        cycles = ld_r_r(m_registers.l, m_registers.l); break;
	case inst_LD_L_HL:       cycles = ld_r_hl(m_registers.l); break;
	case inst_LD_L_A:        cycles = ld_r_r(m_registers.l, m_registers.a); break;

		// 0x70 -> 0x7f
	case inst_LD_HL_B:       cycles = ld_hl_r(m_registers.b); break;
	case inst_LD_HL_C:       cycles = ld_hl_r(m_registers.c); break;
	case inst_LD_HL_D:       cycles = ld_hl_r(m_registers.d); break;
	case inst_LD_HL_E:       cycles = ld_hl_r(m_registers.e); break;
	case inst_LD_HL_H:       cycles = ld_hl_r(m_registers.h); break;
	case inst_LD_HL_L:       cycles = ld_hl_r(m_registers.l); break;
	case inst_HALT:          cycles = halt(); break; // not implemented
	case inst_LD_HL_A:       cycles = ld_hl_r(m_registers.a); break;
	case inst_LD_A_B:        cycles = ld_r_r(m_registers.a, m_registers.b); break;
	case inst_LD_A_C:        cycles = ld_r_r(m_registers.a, m_registers.c); break;
	case inst_LD_A_D:        cycles = ld_r_r(m_registers.a, m_registers.d); break;
	case inst_LD_A_E:        cycles = ld_r_r(m_registers.a, m_registers.e); break;
	case inst_LD_A_H:        cycles = ld_r_r(m_registers.a, m_registers.h); break;
	case inst_LD_A_L:        cycles = ld_r_r(m_registers.a, m_registers.l); break;
	case inst_LD_A_HL:       cycles = ld_r_hl(m_registers.a); break;
	case inst_LD_A_A:        cycles = ld_r_r(m_registers.a, m_registers.a); break;

		// 0x80 -> 0x8f
	case inst_ADD_A_B:		 cycles = add_r(m_registers.b); break;
	case inst_ADD_A_C:		 cycles = add_r(m_registers.c); break;
	case inst_ADD_A_D:		 cycles = add_r(m_registers.d); break;
	case inst_ADD_A_E:		 cycles = add_r(m_registers.e); break;
	case inst_ADD_A_H:		 cycles = add_r(m_registers.h); break;
	case inst_ADD_A_L:		 cycles = add_r(m_registers.l); break;
	case inst_ADD_A_HL:		 cycles = add_hl(); break;
	case inst_ADD_A_A:		 cycles = add_r(m_registers.a); break;
	case inst_ADC_A_B:		 cycles = adc_r(m_registers.b); break;
	case inst_ADC_A_C:		 cycles = adc_r(m_registers.c); break;
	case inst_ADC_A_D:		 cycles = adc_r(m_registers.d); break;
	case inst_ADC_A_E:		 cycles = adc_r(m_registers.e); break;
	case inst_ADC_A_H:		 cycles = adc_r(m_registers.h); break;
	case inst_ADC_A_L:		 cycles = adc_r(m_registers.l); break;
	case inst_ADC_A_HL:		 cycles = adc_hl(); break;
	case inst_ADC_A_A:		 cycles = adc_r(m_registers.a); break;

		// 0x90 -> 0x9f
	case inst_SUB_A_B:		 cycles = sub_r(m_registers.b); break;
	case inst_SUB_A_C:	     cycles = sub_r(m_registers.c); break;
	case inst_SUB_A_D:		 cycles = sub_r(m_registers.d); break;
	case inst_SUB_A_E:		 cycles = sub_r(m_registers.e); break;
	case inst_SUB_A_H:		 cycles = sub_r(m_registers.h); break;
	case inst_SUB_A_L:		 cycles = sub_r(m_registers.l); break;
	case inst_SUB_A_HL:		 cycles = sub_hl(); break;
	case inst_SUB_A_A:	     cycles = sub_r(m_registers.a); break;
	case inst_SBC_A_B:		 cycles = sbc_r(m_registers.b); break;
	case inst_SBC_A_C:		 cycles = sbc_r(m_registers.c); break;
	case inst_SBC_A_D:		 cycles = sbc_r(m_registers.d); break;
	case inst_SBC_A_E:		 cycles = sbc_r(m_registers.e); break;
	case inst_SBC_A_H:		 cycles = sbc_r(m_registers.h); break;
	case inst_SBC_A_L:		 cycles = sbc_r(m_registers.l); break;
	case inst_SBC_A_HL:		 cycles = sbc_hl(); break;
	case inst_SBC_A_A:		 cycles = sbc_r(m_registers.a); break;

		// 0xa0 -> 0xaf
	case inst_AND_A_B:		 cycles = and_r(m_registers.b); break;
	case inst_AND_A_C:		 cycles = and_r(m_registers.c); break;
	case inst_AND_A_D:		 cycles = and_r(m_registers.d); break;
	case inst_AND_A_E:		 cycles = and_r(m_registers.e); break;
	case inst_AND_A_H:		 cycles = and_r(m_registers.h); break;
	case inst_AND_A_L:		 cycles = and_r(m_registers.l); break;
	case inst_AND_A_HL:		 cycles = and_hl(); break;
	case inst_AND_A_A:		 cycles = and_r(m_registers.a); break;
	case inst_XOR_A_B:		 cycles = xor_r(m_registers.b); break;
	case inst_XOR_A_C:		 cycles = xor_r(m_registers.c); break;
	case inst_XOR_A_D:		 cycles = xor_r(m_registers.d); break;
	case inst_XOR_A_E:		 cycles = xor_r(m_registers.e); break;
	case inst_XOR_A_H:		 cycles = xor_r(m_registers.h); break;
	case inst_XOR_A_L:		 cycles = xor_r(m_registers.l); break;
	case inst_XOR_A_HL:		 cycles = xor_hl(); break;
	case inst_XOR_A_A:		 cycles = xor_r(m_registers.a); break;

		// 0xb0 -> 0xbf
	case inst_OR_A_B:		 cycles = or_r(m_registers.b); break;
	case inst_OR_A_C:		 cycles = or_r(m_registers.c); break;
	case inst_OR_A_D:		 cycles = or_r(m_registers.d); break;
	case inst_OR_A_E:		 cycles = or_r(m_registers.e); break;
	case inst_OR_A_H:		 cycles = or_r(m_registers.h); break;
	case inst_OR_A_L:		 cycles = or_r(m_registers.l); break;
	case inst_OR_A_HL:		 cycles = or_hl(); break;
	case inst_OR_A_A:		 cycles = or_r(m_registers.a); break;
	case inst_CP_A_B:		 cycles = cp_r(m_registers.b); break;
	case inst_CP_A_C:		 cycles = cp_r(m_registers.c); break;
	case inst_CP_A_D:		 cycles = cp_r(m_registers.d); break;
	case inst_CP_A_E:		 cycles = cp_r(m_registers.e); break;
	case inst_CP_A_H:		 cycles = cp_r(m_registers.h); break;
	case inst_CP_A_L:		 cycles = cp_r(m_registers.l); break;
	case inst_CP_A_HL:		 cycles = cp_hl(); break;
	case inst_CP_A_A:		 cycles = cp_r(m_registers.a); break;

		// 0xc0 -> 0xcf
	case inst_RET_NZ:        cycles = ret_cc(!get_flag(fZERO)); break;
	case inst_POP_BC:        cycles = pop_rr(rBC); break;
	case inst_JP_NZ_NN:      cycles = jp_cc(!get_flag(fZERO)); break;
	case inst_JP_NN:         cycles = jp_nn(); break;
	case inst_CALL_NZ_NN:    cycles = call_cc(!get_flag(fZERO)); break;
	case inst_PUSH_BC:       cycles = push_rr(rBC); break;
	case inst_ADD_A_N:       cycles = add_n(); break;
	case inst_RST_00:        cycles = rst_n(0x00); break;
	case inst_RET_Z:         cycles = ret_cc(get_flag(fZERO)); break;
	case inst_RET:           cycles = ret(); break;
	case inst_JP_Z_NN:       cycles = jp_cc(get_flag(fZERO)); break;
	case inst_CB:            cycles = execute_cb_opcode(cycles); break;
	case inst_CALL_Z_NN:     cycles = call_cc(get_flag(fZERO)); break;
	case inst_CALL_NN:       cycles = call_nn(); break;
	case inst_ADC_A_N:       cycles = adc_n(); break;
	case inst_RST_08:        cycles = rst_n(0x08); break;

		// 0xd0 -> 0xdf
	case inst_RET_NC:        cycles = ret_cc(!get_flag(fCARRY)); break;
	case inst_POP_DE:        cycles = pop_rr(rDE); break;
	case inst_JP_NC_NN:      cycles = jp_cc(!get_flag(fCARRY)); break;
	case inst_CALL_NC_NN:    cycles = call_cc(!get_flag(fCARRY)); break;
	case inst_PUSH_DE:       cycles = push_rr(rDE); break;
	case inst_SUB_A_N:       cycles = sub_n(); break;
	case inst_RST_10:        cycles = rst_n(0x10); break;
	case inst_RET_C:         cycles = ret_cc(get_flag(fCARRY)); break;
	case inst_RETI:          cycles = reti(); break;
	case inst_JP_C_NN:       cycles = jp_cc(get_flag(fCARRY)); break;
	case inst_CALL_C_NN:     cycles = call_cc(get_flag(fCARRY)); break;
	case inst_SBC_A_N:       cycles = sbc_n(); break;
	case inst_RST_18:        cycles = rst_n(0x18); break;

		// 0xe0 -> 0xef
	case inst_LDH_N_A:       cycles = ldh_n_a(); break;
	case inst_POP_HL:        cycles = pop_rr(rHL); break;
	case inst_LDH_C_A:       cycles = ldh_c_a(); break;
	case inst_PUSH_HL:       cycles = push_rr(rHL); break;
	case inst_AND_A_N:       cycles = and_n(); break;
	case inst_RST_20:        cycles = rst_n(0x20); break;
	case inst_ADD_SP_E:      cycles = add_sp_e(); break;
	case inst_JP_HL:         cycles = jp_hl(); break;
	case inst_LD_NN_A:       cycles = ld_nn_a(); break;
	case inst_XOR_A_N:       cycles = xor_n(); break;
	case inst_RST_28:        cycles = rst_n(0x28); break;

		// 0xf0 -> 0xff
	case inst_LDH_A_N:       cycles = ldh_a_n(); break;
	case inst_POP_AF:        cycles = pop_rr(rAF); break;
	case inst_LDH_A_C:       cycles = ldh_a_c(); break;
	case inst_DI:            cycles = di(); break;
	case inst_PUSH_AF:       cycles = push_rr(rAF); break;
	case inst_OR_A_N:        cycles = or_n(); break;
	case inst_RST_30:        cycles = rst_n(0x30); break;
	case inst_LD_HL_SP_E:    cycles = ld_hl_sp_offset(); break;
	case inst_LD_SP_HL:      cycles = ld_sp_hl(); break;
	case inst_LD_A_NN:       cycles = ld_a_nn(); break;
	case inst_EI:            cycles = ei(); break;
	case inst_CP_A_N:        cycles = cp_n(); break;
	case inst_RST_38:        cycles = rst_n(0x38); break;

	default:
		printf("unimplemented opcode: %02x\n", opcode);
		break;
	}

	return cycles;
}

int bCPU::execute_cb_opcode(int cycles) {
	int cycles_cb = cycles;

	u8 cb_opcode = read_pc();

	switch (cb_opcode) {
	case 0x00:
		cycles += 4;
		break;

	default:
		return cycles_cb;
	}

	return cycles_cb;
}

bool bCPU::get_flag(e_flags flag) const {
	return (m_registers.f >> flag) & 0x1;
}

void bCPU::set_flag(e_flags flag, bool state) {
	if (state) {
		m_registers.f |= (state << flag);
	}
	else {
		m_registers.f &= ~(1 << flag);
	}
}

u16 bCPU::get_joined_reg(e_joined_regs reg) const {
	switch (reg) {
	case rAF:
		return (u16)(m_registers.a << 8 | m_registers.f);

	case rBC:
		return (u16)(m_registers.b << 8 | m_registers.c);

	case rDE:
		return (u16)(m_registers.d << 8 | m_registers.e);

	case rHL:
		return (u16)(m_registers.h << 8 | m_registers.l);

	default:
		return 0xffff;
	}
}

void bCPU::set_joined_reg(e_joined_regs reg, u16 value) {
	u8 low = value & 0xff;
	u8 high = value >> 8;

	switch (reg) {
	case rAF:
		m_registers.a = high;
		m_registers.f = low & 0xf0;
		return;

	case rBC:
		m_registers.b = high;
		m_registers.c = low;
		return;

	case rDE:
		m_registers.d = high;
		m_registers.e = low;
		return;

	case rHL:
		m_registers.h = high;
		m_registers.l = low;
		return;

	default:
		return;
	}
}

//VIRTUAL FUNCTIONS
u8 bCPU::read(u16 address) {
	return 0xff;
}

void bCPU::write(u16 address, u8 value) {

}

u8 bCPU::read_pc(bool read_interrupt) {
	return 0xff;
}

u16 bCPU::read_pc_short() {
	return 0xffff;
}

void bCPU::tick_components(int cycles) {

}

void bCPU::idle_cycle() {

}


