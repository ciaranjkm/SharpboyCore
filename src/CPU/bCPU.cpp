#include <CPU/bCPU.h>

//BASE CPU FUNCTIONS FOR OPERATION

int bCPU::execute_next_instruction() {
	u8 pc = read(m_registers.pc);
	u8 pc1 = read(m_registers.pc + 1);
	u8 pc2 = read(m_registers.pc + 2);
	u8 pc3 = read(m_registers.pc + 3);
	/*
	log << std::format(
		"A:{:02X} F:{:02X} B:{:02X} C:{:02X} D:{:02X} E:{:02X} H:{:02X} L:{:02X} "
		"SP:{:04X} PC:{:04X} PCMEM:{:02X},{:02X},{:02X},{:02X}\n",
		m_registers.a, m_registers.f, m_registers.b, m_registers.c,
		m_registers.d, m_registers.e, m_registers.h, m_registers.l,
		m_registers.sp, m_registers.pc, pc, pc1, pc2, pc3
	);
	*/
	int cycles = 0;
	u8 opcode = read_pc();

	cycles = execute_opcode(opcode);
	return cycles;
}

//todo make reset based on boot rom or not
void bCPU::reset(bool using_boot_rom) {
	if (using_boot_rom) {
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
	case inst_LD_B_C:        
		printf("b: %02x\n", m_registers.b);
		cycles = ld_r_r(m_registers.b, m_registers.c); 
		printf("b: %02x\n", m_registers.b);
		break;
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
	int cycles_cb =  cycles;
	u8 cb_opcode = read_pc();

	switch (cb_opcode) {

		//0x00->0x1f
	case inst_CB_RLC_B: cycles_cb += rlc_r(m_registers.b); break;
	case inst_CB_RLC_C: cycles_cb += rlc_r(m_registers.c); break;
	case inst_CB_RLC_D: cycles_cb += rlc_r(m_registers.d); break;
	case inst_CB_RLC_E: cycles_cb += rlc_r(m_registers.e); break;
	case inst_CB_RLC_H: cycles_cb += rlc_r(m_registers.h); break;
	case inst_CB_RLC_L: cycles_cb += rlc_r(m_registers.l); break;
	case inst_CB_RLC_HL: cycles_cb += rlc_hl(); break;
	case inst_CB_RLC_A: cycles_cb += rlc_r(m_registers.a); break;

	case inst_CB_RRC_B: cycles_cb += rrc_r(m_registers.b); break;
	case inst_CB_RRC_C: cycles_cb += rrc_r(m_registers.c); break;
	case inst_CB_RRC_D: cycles_cb += rrc_r(m_registers.d); break;
	case inst_CB_RRC_E: cycles_cb += rrc_r(m_registers.e); break;
	case inst_CB_RRC_H: cycles_cb += rrc_r(m_registers.h); break;
	case inst_CB_RRC_L: cycles_cb += rrc_r(m_registers.l); break;
	case inst_CB_RRC_HL: cycles_cb += rrc_hl(); break;
	case inst_CB_RRC_A: cycles_cb += rrc_r(m_registers.a); break;

		//0x10->0x1f
	case inst_CB_RL_B: cycles_cb += rl_r(m_registers.b); break;
	case inst_CB_RL_C: cycles_cb += rl_r(m_registers.c); break;
	case inst_CB_RL_D: cycles_cb += rl_r(m_registers.d); break;
	case inst_CB_RL_E: cycles_cb += rl_r(m_registers.e); break;
	case inst_CB_RL_H: cycles_cb += rl_r(m_registers.h); break;
	case inst_CB_RL_L: cycles_cb += rl_r(m_registers.l); break;
	case inst_CB_RL_HL: cycles_cb += rl_hl(); break;
	case inst_CB_RL_A: cycles_cb += rl_r(m_registers.a); break;

	case inst_CB_RR_B: cycles_cb += rr_r(m_registers.b); break;
	case inst_CB_RR_C: cycles_cb += rr_r(m_registers.c); break;
	case inst_CB_RR_D: cycles_cb += rr_r(m_registers.d); break;
	case inst_CB_RR_E: cycles_cb += rr_r(m_registers.e); break;
	case inst_CB_RR_H: cycles_cb += rr_r(m_registers.h); break;
	case inst_CB_RR_L: cycles_cb += rr_r(m_registers.l); break;
	case inst_CB_RR_HL: cycles_cb += rr_hl(); break;
	case inst_CB_RR_A: cycles_cb += rr_r(m_registers.a); break;

		//0x20->0x2f
	case inst_CB_SLA_B: cycles_cb += sla_r(m_registers.b); break;
	case inst_CB_SLA_C: cycles_cb += sla_r(m_registers.c); break;
	case inst_CB_SLA_D: cycles_cb += sla_r(m_registers.d); break;
	case inst_CB_SLA_E: cycles_cb += sla_r(m_registers.e); break;
	case inst_CB_SLA_H: cycles_cb += sla_r(m_registers.h); break;
	case inst_CB_SLA_L: cycles_cb += sla_r(m_registers.l); break;
	case inst_CB_SLA_HL: cycles_cb += sla_hl(); break;
	case inst_CB_SLA_A: cycles_cb += sla_r(m_registers.a); break;

	case inst_CB_SRA_B: cycles_cb += sra_r(m_registers.b); break;
	case inst_CB_SRA_C: cycles_cb += sra_r(m_registers.c); break;
	case inst_CB_SRA_D: cycles_cb += sra_r(m_registers.d); break;
	case inst_CB_SRA_E: cycles_cb += sra_r(m_registers.e); break;
	case inst_CB_SRA_H: cycles_cb += sra_r(m_registers.h); break;
	case inst_CB_SRA_L: cycles_cb += sra_r(m_registers.l); break;
	case inst_CB_SRA_HL: cycles_cb += sra_hl(); break;
	case inst_CB_SRA_A: cycles_cb += sra_r(m_registers.a); break;

		//0x30->0x3f
	case inst_CB_SWAP_B: cycles_cb += swap_r(m_registers.b); break;
	case inst_CB_SWAP_C: cycles_cb += swap_r(m_registers.c); break;
	case inst_CB_SWAP_D: cycles_cb += swap_r(m_registers.d); break;
	case inst_CB_SWAP_E: cycles_cb += swap_r(m_registers.e); break;
	case inst_CB_SWAP_H: cycles_cb += swap_r(m_registers.h); break;
	case inst_CB_SWAP_L: cycles_cb += swap_r(m_registers.l); break;
	case inst_CB_SWAP_HL: cycles_cb += swap_hl(); break;
	case inst_CB_SWAP_A: cycles_cb += swap_r(m_registers.a); break;

	case inst_CB_SRL_B: cycles_cb += srl_r(m_registers.b); break;
	case inst_CB_SRL_C: cycles_cb += srl_r(m_registers.c); break;
	case inst_CB_SRL_D: cycles_cb += srl_r(m_registers.d); break;
	case inst_CB_SRL_E: cycles_cb += srl_r(m_registers.e); break;
	case inst_CB_SRL_H: cycles_cb += srl_r(m_registers.h); break;
	case inst_CB_SRL_L: cycles_cb += srl_r(m_registers.l); break;
	case inst_CB_SRL_HL: cycles_cb += srl_hl(); break;
	case inst_CB_SRL_A: cycles_cb += srl_r(m_registers.a); break;

		//0x40->0x4f
	case inst_CB_BIT0_B: cycles_cb += bit_b_r(0, m_registers.b); break;
	case inst_CB_BIT0_C: cycles_cb += bit_b_r(0, m_registers.c); break;
	case inst_CB_BIT0_D: cycles_cb += bit_b_r(0, m_registers.d); break;
	case inst_CB_BIT0_E: cycles_cb += bit_b_r(0, m_registers.e); break;
	case inst_CB_BIT0_H: cycles_cb += bit_b_r(0, m_registers.h); break;
	case inst_CB_BIT0_L: cycles_cb += bit_b_r(0, m_registers.l); break;
	case inst_CB_BIT0_HL: cycles_cb += bit_b_hl(0); break;
	case inst_CB_BIT0_A: cycles_cb += bit_b_r(0, m_registers.a); break;

	case inst_CB_BIT1_B: cycles_cb += bit_b_r(1, m_registers.b); break;
	case inst_CB_BIT1_C: cycles_cb += bit_b_r(1, m_registers.c); break;
	case inst_CB_BIT1_D: cycles_cb += bit_b_r(1, m_registers.d); break;
	case inst_CB_BIT1_E: cycles_cb += bit_b_r(1, m_registers.e); break;
	case inst_CB_BIT1_H: cycles_cb += bit_b_r(1, m_registers.h); break;
	case inst_CB_BIT1_L: cycles_cb += bit_b_r(1, m_registers.l); break;
	case inst_CB_BIT1_HL: cycles_cb += bit_b_hl(1); break;
	case inst_CB_BIT1_A: cycles_cb += bit_b_r(1, m_registers.a); break;

		//0x50->0x5f
	case inst_CB_BIT2_B: cycles_cb += bit_b_r(2, m_registers.b); break;
	case inst_CB_BIT2_C: cycles_cb += bit_b_r(2, m_registers.c); break;
	case inst_CB_BIT2_D: cycles_cb += bit_b_r(2, m_registers.d); break;
	case inst_CB_BIT2_E: cycles_cb += bit_b_r(2, m_registers.e); break;
	case inst_CB_BIT2_H: cycles_cb += bit_b_r(2, m_registers.h); break;
	case inst_CB_BIT2_L: cycles_cb += bit_b_r(2, m_registers.l); break;
	case inst_CB_BIT2_HL: cycles_cb += bit_b_hl(2); break;
	case inst_CB_BIT2_A: cycles_cb += bit_b_r(2, m_registers.a); break;

	case inst_CB_BIT3_B: cycles_cb += bit_b_r(3, m_registers.b); break;
	case inst_CB_BIT3_C: cycles_cb += bit_b_r(3, m_registers.c); break;
	case inst_CB_BIT3_D: cycles_cb += bit_b_r(3, m_registers.d); break;
	case inst_CB_BIT3_E: cycles_cb += bit_b_r(3, m_registers.e); break;
	case inst_CB_BIT3_H: cycles_cb += bit_b_r(3, m_registers.h); break;
	case inst_CB_BIT3_L: cycles_cb += bit_b_r(3, m_registers.l); break;
	case inst_CB_BIT3_HL: cycles_cb += bit_b_hl(3); break;
	case inst_CB_BIT3_A: cycles_cb += bit_b_r(3, m_registers.a); break;

		//0x60->0x6f
	case inst_CB_BIT4_B: cycles_cb += bit_b_r(4, m_registers.b); break;
	case inst_CB_BIT4_C: cycles_cb += bit_b_r(4, m_registers.c); break;
	case inst_CB_BIT4_D: cycles_cb += bit_b_r(4, m_registers.d); break;
	case inst_CB_BIT4_E: cycles_cb += bit_b_r(4, m_registers.e); break;
	case inst_CB_BIT4_H: cycles_cb += bit_b_r(4, m_registers.h); break;
	case inst_CB_BIT4_L: cycles_cb += bit_b_r(4, m_registers.l); break;
	case inst_CB_BIT4_HL: cycles_cb += bit_b_hl(4); break;
	case inst_CB_BIT4_A: cycles_cb += bit_b_r(4, m_registers.a); break;

	case inst_CB_BIT5_B: cycles_cb += bit_b_r(5, m_registers.b); break;
	case inst_CB_BIT5_C: cycles_cb += bit_b_r(5, m_registers.c); break;
	case inst_CB_BIT5_D: cycles_cb += bit_b_r(5, m_registers.d); break;
	case inst_CB_BIT5_E: cycles_cb += bit_b_r(5, m_registers.e); break;
	case inst_CB_BIT5_H: cycles_cb += bit_b_r(5, m_registers.h); break;
	case inst_CB_BIT5_L: cycles_cb += bit_b_r(5, m_registers.l); break;
	case inst_CB_BIT5_HL: cycles_cb += bit_b_hl(5); break;
	case inst_CB_BIT5_A: cycles_cb += bit_b_r(5, m_registers.a); break;

		//0x70->0x7f
	case inst_CB_BIT6_B: cycles_cb += bit_b_r(6, m_registers.b); break;
	case inst_CB_BIT6_C: cycles_cb += bit_b_r(6, m_registers.c); break;
	case inst_CB_BIT6_D: cycles_cb += bit_b_r(6, m_registers.d); break;
	case inst_CB_BIT6_E: cycles_cb += bit_b_r(6, m_registers.e); break;
	case inst_CB_BIT6_H: cycles_cb += bit_b_r(6, m_registers.h); break;
	case inst_CB_BIT6_L: cycles_cb += bit_b_r(6, m_registers.l); break;
	case inst_CB_BIT6_HL: cycles_cb += bit_b_hl(6); break;
	case inst_CB_BIT6_A: cycles_cb += bit_b_r(6, m_registers.a); break;

	case inst_CB_BIT7_B: cycles_cb += bit_b_r(7, m_registers.b); break;
	case inst_CB_BIT7_C: cycles_cb += bit_b_r(7, m_registers.c); break;
	case inst_CB_BIT7_D: cycles_cb += bit_b_r(7, m_registers.d); break;
	case inst_CB_BIT7_E: cycles_cb += bit_b_r(7, m_registers.e); break;
	case inst_CB_BIT7_H: cycles_cb += bit_b_r(7, m_registers.h); break;
	case inst_CB_BIT7_L: cycles_cb += bit_b_r(7, m_registers.l); break;
	case inst_CB_BIT7_HL: cycles_cb += bit_b_hl(7); break;
	case inst_CB_BIT7_A: cycles_cb += bit_b_r(7, m_registers.a); break;

		//0x80->0x8f
	case inst_CB_RES0_B: cycles_cb += res_b_r(0, m_registers.b); break;
	case inst_CB_RES0_C: cycles_cb += res_b_r(0, m_registers.c); break;
	case inst_CB_RES0_D: cycles_cb += res_b_r(0, m_registers.d); break;
	case inst_CB_RES0_E: cycles_cb += res_b_r(0, m_registers.e); break;
	case inst_CB_RES0_H: cycles_cb += res_b_r(0, m_registers.h); break;
	case inst_CB_RES0_L: cycles_cb += res_b_r(0, m_registers.l); break;
	case inst_CB_RES0_HL: cycles_cb += res_b_hl(0); break;
	case inst_CB_RES0_A: cycles_cb += res_b_r(0, m_registers.a); break;

	case inst_CB_RES1_B: cycles_cb += res_b_r(1, m_registers.b); break;
	case inst_CB_RES1_C: cycles_cb += res_b_r(1, m_registers.c); break;
	case inst_CB_RES1_D: cycles_cb += res_b_r(1, m_registers.d); break;
	case inst_CB_RES1_E: cycles_cb += res_b_r(1, m_registers.e); break;
	case inst_CB_RES1_H: cycles_cb += res_b_r(1, m_registers.h); break;
	case inst_CB_RES1_L: cycles_cb += res_b_r(1, m_registers.l); break;
	case inst_CB_RES1_HL: cycles_cb += res_b_hl(1); break;
	case inst_CB_RES1_A: cycles_cb += res_b_r(1, m_registers.a); break;

		//0x90->0x9f
	case inst_CB_RES2_B: cycles_cb += res_b_r(2, m_registers.b); break;
	case inst_CB_RES2_C: cycles_cb += res_b_r(2, m_registers.c); break;
	case inst_CB_RES2_D: cycles_cb += res_b_r(2, m_registers.d); break;
	case inst_CB_RES2_E: cycles_cb += res_b_r(2, m_registers.e); break;
	case inst_CB_RES2_H: cycles_cb += res_b_r(2, m_registers.h); break;
	case inst_CB_RES2_L: cycles_cb += res_b_r(2, m_registers.l); break;
	case inst_CB_RES2_HL: cycles_cb += res_b_hl(2); break;
	case inst_CB_RES2_A: cycles_cb += res_b_r(2, m_registers.a); break;

	case inst_CB_RES3_B: cycles_cb += res_b_r(3, m_registers.b); break;
	case inst_CB_RES3_C: cycles_cb += res_b_r(3, m_registers.c); break;
	case inst_CB_RES3_D: cycles_cb += res_b_r(3, m_registers.d); break;
	case inst_CB_RES3_E: cycles_cb += res_b_r(3, m_registers.e); break;
	case inst_CB_RES3_H: cycles_cb += res_b_r(3, m_registers.h); break;
	case inst_CB_RES3_L: cycles_cb += res_b_r(3, m_registers.l); break;
	case inst_CB_RES3_HL: cycles_cb += res_b_hl(3); break;
	case inst_CB_RES3_A: cycles_cb += res_b_r(3, m_registers.a); break;

		//0xa0->0xaf
	case inst_CB_RES4_B: cycles_cb += res_b_r(4, m_registers.b); break;
	case inst_CB_RES4_C: cycles_cb += res_b_r(4, m_registers.c); break;
	case inst_CB_RES4_D: cycles_cb += res_b_r(4, m_registers.d); break;
	case inst_CB_RES4_E: cycles_cb += res_b_r(4, m_registers.e); break;
	case inst_CB_RES4_H: cycles_cb += res_b_r(4, m_registers.h); break;
	case inst_CB_RES4_L: cycles_cb += res_b_r(4, m_registers.l); break;
	case inst_CB_RES4_HL: cycles_cb += res_b_hl(4); break;
	case inst_CB_RES4_A: cycles_cb += res_b_r(4, m_registers.a); break;

	case inst_CB_RES5_B: cycles_cb += res_b_r(5, m_registers.b); break;
	case inst_CB_RES5_C: cycles_cb += res_b_r(5, m_registers.c); break;
	case inst_CB_RES5_D: cycles_cb += res_b_r(5, m_registers.d); break;
	case inst_CB_RES5_E: cycles_cb += res_b_r(5, m_registers.e); break;
	case inst_CB_RES5_H: cycles_cb += res_b_r(5, m_registers.h); break;
	case inst_CB_RES5_L: cycles_cb += res_b_r(5, m_registers.l); break;
	case inst_CB_RES5_HL: cycles_cb += res_b_hl(5); break;
	case inst_CB_RES5_A: cycles_cb += res_b_r(5, m_registers.a); break;
		
		//0xb0->bf
	case inst_CB_RES6_B: cycles_cb += res_b_r(6, m_registers.b); break;
	case inst_CB_RES6_C: cycles_cb += res_b_r(6, m_registers.c); break;
	case inst_CB_RES6_D: cycles_cb += res_b_r(6, m_registers.d); break;
	case inst_CB_RES6_E: cycles_cb += res_b_r(6, m_registers.e); break;
	case inst_CB_RES6_H: cycles_cb += res_b_r(6, m_registers.h); break;
	case inst_CB_RES6_L: cycles_cb += res_b_r(6, m_registers.l); break;
	case inst_CB_RES6_HL: cycles_cb += res_b_hl(6); break;
	case inst_CB_RES6_A: cycles_cb += res_b_r(6, m_registers.a); break;

	case inst_CB_RES7_B: cycles_cb += res_b_r(7, m_registers.b); break;
	case inst_CB_RES7_C: cycles_cb += res_b_r(7, m_registers.c); break;
	case inst_CB_RES7_D: cycles_cb += res_b_r(7, m_registers.d); break;
	case inst_CB_RES7_E: cycles_cb += res_b_r(7, m_registers.e); break;
	case inst_CB_RES7_H: cycles_cb += res_b_r(7, m_registers.h); break;
	case inst_CB_RES7_L: cycles_cb += res_b_r(7, m_registers.l); break;
	case inst_CB_RES7_HL: cycles_cb += res_b_hl(7); break;
	case inst_CB_RES7_A: cycles_cb += res_b_r(7, m_registers.a); break;

		//0xc0->0xcf
	case inst_CB_SET0_B: cycles_cb += set_b_r(0, m_registers.b); break;
	case inst_CB_SET0_C: cycles_cb += set_b_r(0, m_registers.c); break;
	case inst_CB_SET0_D: cycles_cb += set_b_r(0, m_registers.d); break;
	case inst_CB_SET0_E: cycles_cb += set_b_r(0, m_registers.e); break;
	case inst_CB_SET0_H: cycles_cb += set_b_r(0, m_registers.h); break;
	case inst_CB_SET0_L: cycles_cb += set_b_r(0, m_registers.l); break;
	case inst_CB_SET0_HL: cycles_cb += set_b_hl(0); break;
	case inst_CB_SET0_A: cycles_cb += set_b_r(0, m_registers.a); break;

	case inst_CB_SET1_B: cycles_cb += set_b_r(1, m_registers.b); break;
	case inst_CB_SET1_C: cycles_cb += set_b_r(1, m_registers.c); break;
	case inst_CB_SET1_D: cycles_cb += set_b_r(1, m_registers.d); break;
	case inst_CB_SET1_E: cycles_cb += set_b_r(1, m_registers.e); break;
	case inst_CB_SET1_H: cycles_cb += set_b_r(1, m_registers.h); break;
	case inst_CB_SET1_L: cycles_cb += set_b_r(1, m_registers.l); break;
	case inst_CB_SET1_HL: cycles_cb += set_b_hl(1); break;
	case inst_CB_SET1_A: cycles_cb += set_b_r(1, m_registers.a); break;

		//0xd0->0xdf
	case inst_CB_SET2_B: cycles_cb += set_b_r(2, m_registers.b); break;
	case inst_CB_SET2_C: cycles_cb += set_b_r(2, m_registers.c); break;
	case inst_CB_SET2_D: cycles_cb += set_b_r(2, m_registers.d); break;
	case inst_CB_SET2_E: cycles_cb += set_b_r(2, m_registers.e); break;
	case inst_CB_SET2_H: cycles_cb += set_b_r(2, m_registers.h); break;
	case inst_CB_SET2_L: cycles_cb += set_b_r(2, m_registers.l); break;
	case inst_CB_SET2_HL: cycles_cb += set_b_hl(2); break;
	case inst_CB_SET2_A: cycles_cb += set_b_r(2, m_registers.a); break;

	case inst_CB_SET3_B: cycles_cb += set_b_r(3, m_registers.b); break;
	case inst_CB_SET3_C: cycles_cb += set_b_r(3, m_registers.c); break;
	case inst_CB_SET3_D: cycles_cb += set_b_r(3, m_registers.d); break;
	case inst_CB_SET3_E: cycles_cb += set_b_r(3, m_registers.e); break;
	case inst_CB_SET3_H: cycles_cb += set_b_r(3, m_registers.h); break;
	case inst_CB_SET3_L: cycles_cb += set_b_r(3, m_registers.l); break;
	case inst_CB_SET3_HL: cycles_cb += set_b_hl(3); break;
	case inst_CB_SET3_A: cycles_cb += set_b_r(3, m_registers.a); break;

		//0xe0->0xef
	case inst_CB_SET4_B: cycles_cb += set_b_r(4, m_registers.b); break;
	case inst_CB_SET4_C: cycles_cb += set_b_r(4, m_registers.c); break;
	case inst_CB_SET4_D: cycles_cb += set_b_r(4, m_registers.d); break;
	case inst_CB_SET4_E: cycles_cb += set_b_r(4, m_registers.e); break;
	case inst_CB_SET4_H: cycles_cb += set_b_r(4, m_registers.h); break;
	case inst_CB_SET4_L: cycles_cb += set_b_r(4, m_registers.l); break;
	case inst_CB_SET4_HL: cycles_cb += set_b_hl(4); break;
	case inst_CB_SET4_A: cycles_cb += set_b_r(4, m_registers.a); break;

	case inst_CB_SET5_B: cycles_cb += set_b_r(5, m_registers.b); break;
	case inst_CB_SET5_C: cycles_cb += set_b_r(5, m_registers.c); break;
	case inst_CB_SET5_D: cycles_cb += set_b_r(5, m_registers.d); break;
	case inst_CB_SET5_E: cycles_cb += set_b_r(5, m_registers.e); break;
	case inst_CB_SET5_H: cycles_cb += set_b_r(5, m_registers.h); break;
	case inst_CB_SET5_L: cycles_cb += set_b_r(5, m_registers.l); break;
	case inst_CB_SET5_HL: cycles_cb += set_b_hl(5); break;
	case inst_CB_SET5_A: cycles_cb += set_b_r(5, m_registers.a); break;

		//0xf0->0xff
	case inst_CB_SET6_B: cycles_cb += set_b_r(6, m_registers.b); break;
	case inst_CB_SET6_C: cycles_cb += set_b_r(6, m_registers.c); break;
	case inst_CB_SET6_D: cycles_cb += set_b_r(6, m_registers.d); break;
	case inst_CB_SET6_E: cycles_cb += set_b_r(6, m_registers.e); break;
	case inst_CB_SET6_H: cycles_cb += set_b_r(6, m_registers.h); break;
	case inst_CB_SET6_L: cycles_cb += set_b_r(6, m_registers.l); break;
	case inst_CB_SET6_HL: cycles_cb += set_b_hl(6); break;
	case inst_CB_SET6_A: cycles_cb += set_b_r(6, m_registers.a); break;

	case inst_CB_SET7_B: cycles_cb += set_b_r(7, m_registers.b); break;
	case inst_CB_SET7_C: cycles_cb += set_b_r(7, m_registers.c); break;
	case inst_CB_SET7_D: cycles_cb += set_b_r(7, m_registers.d); break;
	case inst_CB_SET7_E: cycles_cb += set_b_r(7, m_registers.e); break;
	case inst_CB_SET7_H: cycles_cb += set_b_r(7, m_registers.h); break;
	case inst_CB_SET7_L: cycles_cb += set_b_r(7, m_registers.l); break;
	case inst_CB_SET7_HL: cycles_cb += set_b_hl(7); break;
	case inst_CB_SET7_A: cycles_cb += set_b_r(7, m_registers.a); break;

	default:
		return cycles_cb;
	}
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


