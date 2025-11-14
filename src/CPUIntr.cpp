#include "../include/bCPU.h"

//8 bit load instructions

int bCPU::ld_r_r(u8& dest, u8 src) {
	dest = src;

	return ticks_0;
}

int bCPU::ld_r_n(u8& dest) {
	u8 value = read_pc();
	dest = value;

	return ticks_4;
}

int bCPU::ld_r_hl(u8& dest) {
	u16 hl = get_joined_reg(rHL);
	u8 value = read(hl);
	dest = value;

	return ticks_4;
}

int bCPU::ld_hl_r(u8 src) {
	u16 hl = get_joined_reg(rHL);
	write(hl, src);

	return ticks_4;
}

int bCPU::ld_hl_n() {
	u16 hl = get_joined_reg(rHL);
	u8 value = read_pc();
	write(hl, value);

	return ticks_8;
}

int bCPU::ld_a_bc() {
	u16 bc = get_joined_reg(rBC);
	u8 value = read(bc);
	m_registers.a = value;

	return ticks_4;
}

int bCPU::ld_a_de() {
	u16 de = get_joined_reg(rDE);
	u8 value = read(de);
	m_registers.a = value;

	return ticks_4;
}

int bCPU::ld_bc_a() {
	u16 bc = get_joined_reg(rBC);
	write(bc, m_registers.a);

	return ticks_4;
}

int bCPU::ld_de_a() {
	u16 de = get_joined_reg(rDE);
	write(de, m_registers.a);

	return ticks_4;
}

int bCPU::ld_a_nn() {
	u16 address = read_pc_short();
	u8 value = read(address);

	m_registers.a = value;

	return ticks_12;
}

int bCPU::ld_nn_a() {
	u16 address = read_pc_short();
	write(address, m_registers.a);

	return ticks_12;
}

int bCPU::ldh_a_c() {
	u16 address = 0xff00 + m_registers.c;
	u8 value = read(address);

	m_registers.a = value;

	return ticks_4;
}

int bCPU::ldh_c_a() {
	u16 address = 0xff00 + m_registers.c;
	write(address, m_registers.a);

	return ticks_4;
}

int bCPU::ldh_a_n() {
	u8 n = read_pc();
	u16 address = 0xff00 + n;
	u8 value = read(address);

	m_registers.a = value;

	return ticks_8;
}

int bCPU::ldh_n_a() {
	u8 n = read_pc();
	u16 address = 0xff00 + n;

	write(address, m_registers.a);

	return ticks_8;
}

int bCPU::ld_a_hl_dec() {
	u16 hl = get_joined_reg(rHL);
	u8 value = read(hl);

	m_registers.a = value;
	set_joined_reg(rHL, --hl);

	return ticks_4;
}

int bCPU::ld_hl_dec_a() {
	u16 hl = get_joined_reg(rHL);
	write(hl, m_registers.a);
	set_joined_reg(rHL, --hl);

	return ticks_4;
}

int bCPU::ld_a_hl_inc() {
	u16 hl = get_joined_reg(rHL);
	u8 value = read(hl);

	m_registers.a = value;
	set_joined_reg(rHL, ++hl);

	return ticks_4;
}

int bCPU::ld_hl_inc_a() {
	u16 hl = get_joined_reg(rHL);
	write(hl, m_registers.a);
	set_joined_reg(rHL, hl + 1);

	return ticks_4;
}

//16 bit load instructions

int bCPU::ld_rr_nn(e_joined_regs dest) {
	u16 value = read_pc_short();
	set_joined_reg(dest, value);

	return ticks_8;
}

int bCPU::ld_rr_nn(u16& dest) {
	u16 value = read_pc_short();
	dest = value;

	return ticks_8;
}

int bCPU::ld_nn_sp() {
	u16 nn = read_pc_short();

	u8 sp_low = m_registers.sp & 0x00ff;
	u8 sp_high = m_registers.sp >> 8;

	write(nn, sp_low);
	write(nn + 1, sp_high);

	return ticks_16;
}

int bCPU::ld_sp_hl() {
	u16 hl = get_joined_reg(rHL);

	idle_cycle();
	m_registers.sp = hl;

	return ticks_4;
}

int bCPU::push_rr(e_joined_regs src) {
	u16 rr = get_joined_reg(src);

	u8 rr_high = rr >> 8;
	u8 rr_low = rr & 0x00ff;

	m_registers.sp--;
	idle_cycle();

	write(m_registers.sp, rr_high);
	m_registers.sp--;

	write(m_registers.sp, rr_low);

	return ticks_12;
}

int bCPU::pop_rr(e_joined_regs src) {
	u8 rr_low = read(m_registers.sp);
	m_registers.sp++;

	u8 rr_high = read(m_registers.sp);
	m_registers.sp++;

	u16 rr = (rr_high << 8) | rr_low;
	set_joined_reg(src, rr);

	return ticks_8;
}

int bCPU::ld_hl_sp_offset() {
	u8 sp_low = m_registers.sp & 0x00ff;
	u8 sp_high = m_registers.sp >> 8;

	u8 offset = read_pc();

	u8 l_result = sp_low + offset;
	m_registers.l = (u8)l_result;

	set_flag(fZERO, false);
	set_flag(fSUB, false);
	set_flag(fHCARRY, (sp_low & 0x0f) + (offset & 0x0f) > 0x0f);
	set_flag(fCARRY, (sp_low + offset) > 0xff);

	bool z_sign = (s8)offset < 0;
	u8 adjustment = z_sign ? 0xff : 0x00;

	idle_cycle();

	u8 h_result = sp_high + adjustment + get_flag(fCARRY);
	m_registers.h = (u8)h_result;

	return ticks_8;
}

//8 bit arithmetic and logic instructions
int bCPU::add_r(u8 src) {
	u8 result = m_registers.a + src;

	set_flag(fZERO, result == 0x00);
	set_flag(fSUB, false);
	set_flag(fHCARRY, (m_registers.a & 0x0f) + (src & 0x0f) > 0x0f);
	set_flag(fCARRY, m_registers.a + src > 0xff);

	m_registers.a = result;
	return ticks_0;
}

int bCPU::add_hl() {
	u16 hl = get_joined_reg(rHL);
	u8 src = read(hl);

	u8 result = m_registers.a + src;

	set_flag(fZERO, result == 0x00);
	set_flag(fSUB, false);
	set_flag(fHCARRY, (m_registers.a & 0x0f) + (src & 0x0f) > 0x0f);
	set_flag(fCARRY, m_registers.a + src > 0xff);

	m_registers.a = result;
	return ticks_4;
}

int bCPU::add_n() {
	u8 src = read_pc();

	u8 result = m_registers.a + src;

	set_flag(fZERO, result == 0x00);
	set_flag(fSUB, false);
	set_flag(fHCARRY, (m_registers.a & 0x0f) + (src & 0x0f) > 0x0f);
	set_flag(fCARRY, m_registers.a + src > 0xff);

	m_registers.a = result;
	return ticks_4;
}

int bCPU::adc_r(u8 src) {
	bool c_flag = get_flag(fCARRY);
	u8 result = m_registers.a + src + c_flag;

	bool new_half_carry = (m_registers.a & 0x0f) + (src & 0x0f) + c_flag > 0x0f;
	bool new_carry = (m_registers.a + src + c_flag) > 0xff;
	m_registers.a = result;

	set_flag(fZERO, result == 0x00);
	set_flag(fSUB, false);
	set_flag(fHCARRY, new_half_carry);
	set_flag(fCARRY, new_carry);

	return ticks_0;
}

int bCPU::adc_hl() {
	u16 hl = get_joined_reg(rHL);
	u8 src = read(hl);

	bool c_flag = get_flag(fCARRY);
	u8 result = m_registers.a + src + get_flag(fCARRY);

	bool new_half_carry = (m_registers.a & 0x0f) + (src & 0x0f) + c_flag > 0x0f;
	bool new_carry = (m_registers.a + src + c_flag) > 0xff;
	m_registers.a = result;

	set_flag(fZERO, result == 0x00);
	set_flag(fSUB, false);
	set_flag(fHCARRY, new_half_carry);
	set_flag(fCARRY, new_carry);


	return ticks_4;
}

int bCPU::adc_n() {
	u8 src = read_pc();

	bool c_flag = get_flag(fCARRY);
	u8 result = m_registers.a + src + get_flag(fCARRY);

	bool new_half_carry = (m_registers.a & 0x0f) + (src & 0x0f) + c_flag > 0x0f;
	bool new_carry = (m_registers.a + src + c_flag) > 0xff;
	m_registers.a = result;

	set_flag(fZERO, result == 0x00);
	set_flag(fSUB, false);
	set_flag(fHCARRY, new_half_carry);
	set_flag(fCARRY, new_carry);

	return ticks_4;
}

int bCPU::sub_r(u8 src) {
	u8 result = m_registers.a - src;

	bool new_half_carry = (m_registers.a & 0x0f) < (src & 0x0f);
	bool new_carry = m_registers.a < src;

	m_registers.a = result;

	set_flag(fZERO, result == 0x00);
	set_flag(fSUB, true);
	set_flag(fHCARRY, new_half_carry);
	set_flag(fCARRY, new_carry);

	return ticks_0;
}

int bCPU::sub_hl() {
	u16 hl = get_joined_reg(rHL);
	u8 src = read(hl);

	u8 result = m_registers.a - src;

	bool new_half_carry = (m_registers.a & 0x0f) < (src & 0x0f);
	bool new_carry = m_registers.a < src;

	m_registers.a = result;

	set_flag(fZERO, result == 0x00);
	set_flag(fSUB, true);
	set_flag(fHCARRY, new_half_carry);
	set_flag(fCARRY, new_carry);

	return ticks_4;
}

int bCPU::sub_n() {
	u8 src = read_pc();

	u8 result = m_registers.a - src;

	bool new_half_carry = (m_registers.a & 0x0f) < (src & 0x0f);
	bool new_carry = m_registers.a < src;

	m_registers.a = result;

	set_flag(fZERO, result == 0x00);
	set_flag(fSUB, true);
	set_flag(fHCARRY, new_half_carry);
	set_flag(fCARRY, new_carry);

	return ticks_4;
}

int bCPU::sbc_r(u8 src) {
	bool c_flag = get_flag(fCARRY);
	u8 result = m_registers.a - src - c_flag;

	bool new_half_carry = (m_registers.a & 0x0f) < (src & 0x0f) + c_flag;
	bool new_carry = m_registers.a < src + c_flag;

	m_registers.a = result;

	set_flag(fZERO, result == 0x00);
	set_flag(fSUB, true);
	set_flag(fHCARRY, new_half_carry);
	set_flag(fCARRY, new_carry);

	return ticks_0;
}

int bCPU::sbc_hl() {
	u16 hl = get_joined_reg(rHL);
	u8 src = read(hl);

	bool c_flag = get_flag(fCARRY);
	u8 result = m_registers.a - src - c_flag;

	bool new_half_carry = (m_registers.a & 0x0f) < (src & 0x0f) + c_flag;
	bool new_carry = m_registers.a < src + c_flag;

	m_registers.a = result;

	set_flag(fZERO, result == 0x00);
	set_flag(fSUB, true);
	set_flag(fHCARRY, new_half_carry);
	set_flag(fCARRY, new_carry);

	return ticks_4;
}

int bCPU::sbc_n() {
	u8 src = read_pc();

	bool c_flag = get_flag(fCARRY);
	u8 result = m_registers.a - src - c_flag;

	bool new_half_carry = (m_registers.a & 0x0f) < (src & 0x0f) + c_flag;
	bool new_carry = m_registers.a < src + c_flag;

	m_registers.a = result;

	set_flag(fZERO, result == 0x00);
	set_flag(fSUB, true);
	set_flag(fHCARRY, new_half_carry);
	set_flag(fCARRY, new_carry);

	return ticks_4;

}

int bCPU::cp_r(u8 src) {
	u8 result = m_registers.a - src;

	bool new_half_carry = (m_registers.a & 0x0f) < (src & 0x0f);
	bool new_carry = m_registers.a < src;

	set_flag(fZERO, result == 0x00);
	set_flag(fSUB, true);
	set_flag(fHCARRY, new_half_carry);
	set_flag(fCARRY, new_carry);

	return ticks_0;
}

int bCPU::cp_hl() {
	u16 hl = get_joined_reg(rHL);
	u8 src = read(hl);

	u8 result = m_registers.a - src;

	bool new_half_carry = (m_registers.a & 0x0f) < (src & 0x0f);
	bool new_carry = m_registers.a < src;

	set_flag(fZERO, result == 0x00);
	set_flag(fSUB, true);
	set_flag(fHCARRY, new_half_carry);
	set_flag(fCARRY, new_carry);

	return ticks_4;
}

int bCPU::cp_n() {
	u8 src = read_pc();

	u8 result = m_registers.a - src;

	bool new_half_carry = (m_registers.a & 0x0f) < (src & 0x0f);
	bool new_carry = m_registers.a < src;

	set_flag(fZERO, result == 0x00);
	set_flag(fSUB, true);
	set_flag(fHCARRY, new_half_carry);
	set_flag(fCARRY, new_carry);

	return ticks_4;
}

int bCPU::inc_r(u8& src) {
	u8 result = src + 0x01;

	bool new_half_carry = (src & 0x0f) + 0x01 > 0x0f;

	src = result;

	set_flag(fZERO, result == 0);
	set_flag(fSUB, false);
	set_flag(fHCARRY, new_half_carry);

	return ticks_0;
}

int bCPU::inc_hl() {
	u16 hl = get_joined_reg(rHL);
	u8 src = read(hl);

	u8 result = src + 1;
	bool new_half_carry = (src & 0x0f) + 0x01 > 0x0f;

	write(hl, result);

	set_flag(fZERO, result == 0);
	set_flag(fSUB, false);
	set_flag(fHCARRY, new_half_carry);

	return ticks_8;
}

int bCPU::dec_r(u8& src) {
	u8 result = src - 1;
	bool new_half_carry = (src & 0x0f) < 0x01;

	src = result;

	set_flag(fZERO, result == 0);
	set_flag(fSUB, true);
	set_flag(fHCARRY, new_half_carry);

	return ticks_0;
}

int bCPU::dec_hl() {
	u16 hl = get_joined_reg(rHL);
	u8 src = read(hl);

	u8 result = src - 1;
	bool new_half_carry = (src & 0x0f) < 0x01;

	write(hl, result);

	set_flag(fZERO, result == 0);
	set_flag(fSUB, true);
	set_flag(fHCARRY, new_half_carry);

	return ticks_8;
}

int bCPU::and_r(u8 src) {
	u8 result = m_registers.a & src;
	m_registers.a = result;

	set_flag(fZERO, result == 0);
	set_flag(fSUB, false);
	set_flag(fHCARRY, true);
	set_flag(fCARRY, false);

	return ticks_0;
}

int bCPU::and_hl() {
	u16 hl = get_joined_reg(rHL);
	u8 src = read(hl);

	u8 result = m_registers.a & src;
	m_registers.a = result;

	set_flag(fZERO, result == 0);
	set_flag(fSUB, false);
	set_flag(fHCARRY, true);
	set_flag(fCARRY, false);

	return ticks_4;
}

int bCPU::and_n() {
	u8 src = read_pc();

	u8 result = m_registers.a & src;
	m_registers.a = result;

	set_flag(fZERO, result == 0);
	set_flag(fSUB, false);
	set_flag(fHCARRY, true);
	set_flag(fCARRY, false);

	return ticks_4;
}

int bCPU::or_r(u8 src) {
	u8 result = m_registers.a | src;
	m_registers.a = result;

	set_flag(fZERO, result == 0);
	set_flag(fSUB, false);
	set_flag(fHCARRY, false);
	set_flag(fCARRY, false);

	return ticks_0;
}

int bCPU::or_hl() {
	u16 hl = get_joined_reg(rHL);
	u8 src = read(hl);

	u8 result = m_registers.a | src;
	m_registers.a = result;

	set_flag(fZERO, result == 0);
	set_flag(fSUB, false);
	set_flag(fHCARRY, false);
	set_flag(fCARRY, false);

	return ticks_4;
}

int bCPU::or_n() {
	u8 src = read_pc();

	u8 result = m_registers.a | src;
	m_registers.a = result;

	set_flag(fZERO, result == 0);
	set_flag(fSUB, false);
	set_flag(fHCARRY, false);
	set_flag(fCARRY, false);

	return ticks_4;
}

int bCPU::xor_r(u8 src) {
	u8 result = m_registers.a ^ src;
	m_registers.a = result;

	set_flag(fZERO, result == 0);
	set_flag(fSUB, false);
	set_flag(fHCARRY, false);
	set_flag(fCARRY, false);

	return ticks_0;
}

int bCPU::xor_hl() {
	u16 hl = get_joined_reg(rHL);
	u8 src = read(hl);

	u8 result = m_registers.a ^ src;
	m_registers.a = result;

	set_flag(fZERO, result == 0);
	set_flag(fSUB, false);
	set_flag(fHCARRY, false);
	set_flag(fCARRY, false);

	return ticks_4;
}

int bCPU::xor_n() {
	u8 src = read_pc();

	u8 result = m_registers.a ^ src;
	m_registers.a = result;

	set_flag(fZERO, result == 0);
	set_flag(fSUB, false);
	set_flag(fHCARRY, false);
	set_flag(fCARRY, false);

	return ticks_4;
}

int bCPU::ccf() {
	set_flag(fSUB, false);
	set_flag(fHCARRY, false);
	set_flag(fCARRY, !get_flag(fCARRY));

	return ticks_0;
}

int bCPU::scf() {
	set_flag(fSUB, false);
	set_flag(fHCARRY, false);
	set_flag(fCARRY, true);

	return ticks_0;
}

int bCPU::daa() {
	u8 a = m_registers.a;
	u8 adjustment = 0x00;

	bool set_carry = false;

	bool sub_flag = get_flag(fSUB);
	bool halfcarry_flag = get_flag(fHCARRY);
	bool carry_flag = get_flag(fCARRY);

	if (!sub_flag) {
		if (halfcarry_flag || (m_registers.a & 0x0f) > 0x09) {
			adjustment |= 0x06;
		}
		if (carry_flag || (m_registers.a > 0x99)) {
			adjustment += 0x60;
			set_carry = true;
		}

		a += adjustment;
	}
	else {
		if (halfcarry_flag) {
			adjustment |= 0x06;
		}
		if (carry_flag) {
			adjustment |= 0x60;
		}

		a -= adjustment;
	}

	m_registers.a = a;

	set_flag(fZERO, m_registers.a == 0);
	set_flag(fHCARRY, false);
	set_flag(fCARRY, set_carry || carry_flag);

	return ticks_0;
}

int bCPU::cpl() {
	m_registers.a = ~m_registers.a;

	set_flag(fSUB, true);
	set_flag(fHCARRY, true);

	return ticks_0;
}

//16 bit arithemtic instructions
int bCPU::inc_rr(e_joined_regs dest) {
	u16 rr = get_joined_reg(dest);
	rr += 1;

	idle_cycle();

	set_joined_reg(dest, rr);

	return ticks_4;
}

int bCPU::dec_rr(e_joined_regs dest) {
	u16 rr = get_joined_reg(dest);
	rr -= 1;

	idle_cycle();

	set_joined_reg(dest, rr);

	return ticks_4;
}

int bCPU::add_hl_rr(e_joined_regs src) {
	u16 hl = get_joined_reg(rHL);
	u16 rr = get_joined_reg(src);

	u16 result = hl + rr;

	bool new_halfcarry = (hl & 0x0fff) + (rr & 0x0fff) > 0x0fff;
	bool new_carry = (hl + rr) > 0xffff;

	m_registers.l = (u8)(result & 0xff);
	idle_cycle();

	m_registers.h = (u8)(result >> 8);

	set_flag(fSUB, false);
	set_flag(fHCARRY, new_halfcarry);
	set_flag(fCARRY, new_carry);

	return ticks_4;
}

int bCPU::add_hl_rr(u16 src) {
	u16 hl = get_joined_reg(rHL);

	u16 result = hl + src;

	bool new_halfcarry = (hl & 0x0fff) + (src & 0x0fff) > 0x0fff;
	bool new_carry = (hl + src) > 0xffff;

	m_registers.l = (u8)(result & 0xff);
	idle_cycle();

	m_registers.h = (u8)(result >> 8);

	set_flag(fSUB, false);
	set_flag(fHCARRY, new_halfcarry);
	set_flag(fCARRY, new_carry);

	return ticks_4;
}

int bCPU::add_sp_e() {
	s8 e = read_pc();
	u16 result = m_registers.sp + e;

	u8 sp_low = (u8)(m_registers.sp & 0xff);
	bool new_halfcarry = (sp_low & 0x0f) + ((u8)e & 0x0f) > 0x0f;
	bool new_carry = sp_low + (u8)e > 0xff;

	set_flag(fZERO, false);
	set_flag(fSUB, false);
	set_flag(fHCARRY, new_halfcarry);
	set_flag(fCARRY, new_carry);

	idle_cycle();
	idle_cycle();

	m_registers.sp = result;
	return ticks_12;
}

//rotate shift and bit op instructions
int bCPU::rlca() {
	u8 b7 = (m_registers.a >> 7) & 0x01;
	bool new_carry = b7 != 0;

	m_registers.a = (m_registers.a << 1) | b7;

	set_flag(fZERO, false);
	set_flag(fSUB, false);
	set_flag(fHCARRY, false);
	set_flag(fCARRY, new_carry);

	return ticks_0;
}

int bCPU::rrca() {
	u8 b0 = m_registers.a & 0x01;
	bool new_carry = b0 != 0;

	m_registers.a = (m_registers.a >> 1) | (b0 << 7);

	set_flag(fZERO, false);
	set_flag(fSUB, false);
	set_flag(fHCARRY, false);
	set_flag(fCARRY, new_carry);

	return ticks_0;
}

int bCPU::rla() {
	u8 current_carry = (u8)get_flag(fCARRY);
	u8 b7 = (m_registers.a >> 7) & 0x01;
	bool new_carry = b7 != 0;

	m_registers.a = (m_registers.a << 1) | current_carry;

	set_flag(fZERO, false);
	set_flag(fSUB, false);
	set_flag(fHCARRY, false);
	set_flag(fCARRY, new_carry);

	return ticks_0;
}

int bCPU::rra() {
	u8 current_carry = (u8)get_flag(fCARRY);
	u8 b0 = m_registers.a & 0x01;
	bool new_carry = b0 != 0;

	m_registers.a = (m_registers.a >> 1) | (current_carry << 7);

	set_flag(fZERO, false);
	set_flag(fSUB, false);
	set_flag(fHCARRY, false);
	set_flag(fCARRY, new_carry);

	return ticks_0;
}

int bCPU::rlc_r(u8& src) {
	u8 b7 = (src >> 7) & 0x01;
	bool new_carry = b7 != 0;

	u8 result = (src << 1) | b7;
	src = result;

	set_flag(fZERO, result == 0);
	set_flag(fSUB, false);
	set_flag(fHCARRY, false);
	set_flag(fCARRY, new_carry);

	return ticks_4;
}

int bCPU::rlc_hl() {
	u16 hl = get_joined_reg(rHL);
	u8 src = read(hl);

	u8 b7 = (src >> 7) & 0x01;
	bool new_carry = b7 != 0;

	u8 result = (src << 1) | b7;
	write(hl, result);

	set_flag(fZERO, result == 0);
	set_flag(fSUB, false);
	set_flag(fHCARRY, false);
	set_flag(fCARRY, new_carry);

	return ticks_12;
}

int bCPU::rrc_r(u8& src) {
	u8 b0 = src & 0x01;
	bool new_carry = b0 != 0;

	u8 result = (src >> 1) | (b0 << 7);
	src = result;

	set_flag(fZERO, result == 0);
	set_flag(fSUB, false);
	set_flag(fHCARRY, false);
	set_flag(fCARRY, new_carry);

	return ticks_4;
}

int bCPU::rrc_hl() {
	u16 hl = get_joined_reg(rHL);
	u8 src = read(hl);

	u8 b0 = src & 0x01;
	bool new_carry = b0 != 0;

	u8 result = (src >> 1) | (b0 << 7);
	write(hl, result);

	set_flag(fZERO, result == 0);
	set_flag(fSUB, false);
	set_flag(fHCARRY, false);
	set_flag(fCARRY, new_carry);

	return ticks_12;
}

int bCPU::rl_r(u8& src) {
	u8 b7 = (src >> 7) & 0x01;
	u8 current_carry = (u8)get_flag(fCARRY);
	bool new_carry = b7 != 0;

	u8 result = (src << 1) | current_carry;
	src = result;

	set_flag(fZERO, result == 0);
	set_flag(fSUB, false);
	set_flag(fHCARRY, false);
	set_flag(fCARRY, new_carry);

	return ticks_4;
}

int bCPU::rl_hl() {
	u16 hl = get_joined_reg(rHL);
	u8 src = read(hl);

	u8 b7 = (src >> 7) & 0x01;
	u8 current_carry = (u8)get_flag(fCARRY);
	bool new_carry = b7 != 0;

	u8 result = (src << 1) | current_carry;
	write(hl, result);

	set_flag(fZERO, result == 0);
	set_flag(fSUB, false);
	set_flag(fHCARRY, false);
	set_flag(fCARRY, new_carry);

	return ticks_12;
}

int bCPU::rr_r(u8& src) {
	u8 b0 = src & 0x01;
	u8 current_carry = (u8)get_flag(fCARRY);
	bool new_carry = b0 != 0;

	u8 result = (src >> 1) | (current_carry << 7);
	src = result;

	set_flag(fZERO, result == 0);
	set_flag(fSUB, false);
	set_flag(fHCARRY, false);
	set_flag(fCARRY, new_carry);

	return ticks_4;
}

int bCPU::rr_hl() {
	u16 hl = get_joined_reg(rHL);
	u8 src = read(hl);

	u8 b0 = src & 0x01;
	u8 current_carry = (u8)get_flag(fCARRY);
	bool new_carry = b0 != 0;

	u8 result = (src >> 1) | (current_carry << 7);
	write(hl, result);

	set_flag(fZERO, result == 0);
	set_flag(fSUB, false);
	set_flag(fHCARRY, false);
	set_flag(fCARRY, new_carry);

	return ticks_12;
}

int bCPU::sla_r(u8& src) {
	u8 b7 = (src >> 7) & 0x01;
	bool new_carry = b7 != 0;

	u8 result = (src << 1) | 0x00;
	src = result;

	set_flag(fZERO, result == 0);
	set_flag(fSUB, false);
	set_flag(fHCARRY, false);
	set_flag(fCARRY, new_carry);

	return ticks_4;
}

int bCPU::sla_hl() {
	u16 hl = get_joined_reg(rHL);
	u8 src = read(hl);

	u8 b7 = (src >> 7) & 0x01;
	bool new_carry = b7 != 0;

	u8 result = (src << 1) | 0x00;
	write(hl, result);

	set_flag(fZERO, result == 0);
	set_flag(fSUB, false);
	set_flag(fHCARRY, false);
	set_flag(fCARRY, new_carry);

	return ticks_12;
}

int bCPU::sra_r(u8& src) {
	u8 b7 = (src >> 7) & 0x1;
	u8 b0 = src & 0x1;
	bool new_carry = b0 != 0;

	u8 result = (src >> 1) | (b7 << 7);
	src = result;

	set_flag(fZERO, result == 0);
	set_flag(fSUB, false);
	set_flag(fHCARRY, false);
	set_flag(fCARRY, new_carry);

	return ticks_4;
}

int bCPU::sra_hl() {
	u16 hl = get_joined_reg(rHL);
	u8 src = read(hl);

	u8 b7 = (src >> 7) & 0x1;
	u8 b0 = src & 0x1;
	bool new_carry = b0 != 0;

	u8 result = (src >> 1) | (b7 << 7);
	write(hl, result);

	set_flag(fZERO, result == 0);
	set_flag(fSUB, false);
	set_flag(fHCARRY, false);
	set_flag(fCARRY, new_carry);

	return ticks_12;
}

int bCPU::swap_r(u8& src) {
	u8 result = (src << 4) | (src >> 4);
	src = result;

	set_flag(fZERO, result == 0);
	set_flag(fSUB, false);
	set_flag(fHCARRY, false);
	set_flag(fCARRY, false);

	return ticks_4;
}

int bCPU::swap_hl() {
	u16 hl = get_joined_reg(rHL);
	u8 src = read(hl);

	u8 result = (src << 4) | (src >> 4);
	write(hl, result);

	set_flag(fZERO, result == 0);
	set_flag(fSUB, false);
	set_flag(fHCARRY, false);
	set_flag(fCARRY, false);

	return ticks_12;
}

int bCPU::srl_r(u8& src) {
	u8 b0 = src & 0x1;
	bool new_carry = b0 != 0;

	u8 result = (src >> 1) & (~(0x1 << 7));
	src = result;

	set_flag(fZERO, result == 0);
	set_flag(fSUB, false);
	set_flag(fHCARRY, false);
	set_flag(fCARRY, new_carry);

	return ticks_4;
}

int bCPU::srl_hl() {
	u16 hl = get_joined_reg(rHL);
	u8 src = read(hl);

	u8 b0 = src & 0x1;
	bool new_carry = b0 != 0;

	u8 result = (src >> 1) & (~(0x1 << 7));
	write(hl, result);

	set_flag(fZERO, result == 0);
	set_flag(fSUB, false);
	set_flag(fHCARRY, false);
	set_flag(fCARRY, new_carry);

	return ticks_12;
}

int bCPU::bit_b_r(int bit, u8 src) {
	bool zero = (src & (0x01 << bit)) != 0;

	set_flag(fZERO, !zero);
	set_flag(fSUB, false);
	set_flag(fHCARRY, true);

	return ticks_4;
}

int bCPU::bit_b_hl(int bit) {
	u16 hl = get_joined_reg(rHL);
	u8 src = read(hl);

	bool zero = (src & (0x01 << bit)) != 0;

	set_flag(fZERO, !zero);
	set_flag(fSUB, false);
	set_flag(fHCARRY, true);

	return ticks_8;
}

int bCPU::res_b_r(int bit, u8& src) {
	u8 mask = ~(1 << bit);
	src &= mask;

	return ticks_4;
}

int bCPU::res_b_hl(int bit) {
	u16 hl = get_joined_reg(rHL);
	u8 src = read(hl);

	u8 mask = ~(1 << bit);
	u8 result = src & mask;

	write(hl, result);

	return ticks_12;
}

int bCPU::set_b_r(int bit, u8& src) {
	u8 set = (1 << bit);
	src |= set;

	return ticks_4;
}

int bCPU::set_b_hl(int bit) {
	u16 hl = get_joined_reg(rHL);
	u8 src = read(hl);

	u8 set = (1 << bit);
	u8 result = src | set;
	write(hl, result);

	return ticks_12;
}

//control flow instructions
int bCPU::jp_nn() {
	u8 nn_low = read_pc();
	u8 nn_high = read_pc();

	u16 address = (u16)((nn_high << 8) | nn_low);

	idle_cycle();
	m_registers.pc = address;

	return ticks_12;
}

int bCPU::jp_hl() {
	u16 hl = get_joined_reg(rHL);

	m_registers.pc = hl;

	return ticks_0;
}

int bCPU::jp_cc(bool condition) {
	u8 nn_low = read_pc();
	u8 nn_high = read_pc();

	u16 address = (u16)((nn_high << 8) | nn_low);

	if (condition) {
		idle_cycle();
		m_registers.pc = address;
		return ticks_12;
	}

	return ticks_8;
}

int bCPU::jr_e() {
	s8 e = (s8)read_pc();
	u16 result = m_registers.pc + e;

	idle_cycle();
	m_registers.pc = result;

	return ticks_8;
}

int bCPU::jr_cc(bool condition) {
	s8 e = (s8)read_pc();
	u16 result = m_registers.pc + e;

	if (condition) {
		idle_cycle();
		m_registers.pc = result;

		return ticks_8;
	}

	return ticks_4;
}

int bCPU::call_nn() {
	u8 nn_low = read_pc();
	u8 nn_high = read_pc();

	u16 address = (u16)((nn_high << 8) | nn_low);

	u8 pc_low = m_registers.pc & 0xff;
	u8 pc_high = m_registers.pc >> 8;

	idle_cycle();
	m_registers.sp -= 1;

	write(m_registers.sp, pc_high);
	m_registers.sp -= 1;
	write(m_registers.sp, pc_low);

	m_registers.pc = address;

	return ticks_20;
}

int bCPU::call_cc(bool condition) {
	u8 nn_low = read_pc();
	u8 nn_high = read_pc();

	u16 address = (u16)((nn_high << 8) | nn_low);

	u8 pc_low = m_registers.pc & 0xff;
	u8 pc_high = m_registers.pc >> 8;

	if (condition) {
		idle_cycle();
		m_registers.sp -= 1;

		write(m_registers.sp, pc_high);
		m_registers.sp -= 1;
		write(m_registers.sp, pc_low);

		m_registers.pc = address;

		return ticks_20;
	}

	return ticks_8;
}

int bCPU::ret() {
	u8 ret_low = read(m_registers.sp);
	m_registers.sp += 1;
	u8 ret_high = read(m_registers.sp);
	m_registers.sp += 1;

	u16 address = (u16)((ret_high << 8) | ret_low);

	idle_cycle();
	m_registers.pc = address;

	return ticks_12;
}

int bCPU::ret_cc(bool condition) {
	idle_cycle();

	if (condition) {
		u8 ret_low = read(m_registers.sp);
		m_registers.sp += 1;
		u8 ret_high = read(m_registers.sp);
		m_registers.sp += 1;

		u16 address = (u16)((ret_high << 8) | ret_low);

		idle_cycle();
		m_registers.pc = address;

		return ticks_12;
	}

	return ticks_4;
}

int bCPU::reti() {
	u8 ret_low = read(m_registers.sp);
	m_registers.sp += 1;
	u8 ret_high = read(m_registers.sp);
	m_registers.sp += 1;

	u16 address = (u16)((ret_high << 8) | ret_low);

	idle_cycle();
	m_registers.pc = address;
	m_cpu.ime = true;

	return ticks_12;
}

int bCPU::rst_n(u8 vector) {
	u8 pc_low = m_registers.pc & 0xff;
	u8 pc_high = m_registers.pc >> 8;

	idle_cycle();
	m_registers.sp -= 1;

	write(m_registers.sp, pc_high);
	m_registers.sp -= 1;
	write(m_registers.sp, pc_low);

	m_registers.pc = (u16)(0x0000 | vector);

	return ticks_12;
}

//misc instructions
int bCPU::halt() {
	if (m_cpu.interrupt_pending != 0 && !m_cpu.ime) {
		m_cpu.halt_bug = true;
		m_cpu.halted = false;

		return ticks_0;
	}

	m_cpu.halted = true;

	return ticks_0;
}

int bCPU::stop() {
	//double speed mode cgb
	//not impl on sharpboy for dmg.

	return ticks_0;
}

int bCPU::ei() {
	m_cpu.enable_ime = true;

	return ticks_0;
}

int bCPU::di() {
	m_cpu.ime = false;

	return ticks_0;
}