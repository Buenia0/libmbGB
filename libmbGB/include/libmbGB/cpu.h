// This file is part of libmbGB.
// Copyright (C) 2019 Buenia.
// libmbGB is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// libmbGB is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with libmbGB.  If not, see <https://www.gnu.org/licenses/>.

#ifndef LIBMBGB_CPU
#define LIBMBGB_CPU

#include "mmu.h"
#include "gpu.h"
#include "timers.h"
#include "serial.h"
#include "libmbgb_api.h"
#include <cstdint>
#include <iostream>
using namespace gb;
using namespace std;

namespace gb
{
    struct Register
    {
	uint8_t hi;
	uint8_t lo;
	uint16_t reg;

	uint8_t gethi()
	{
	    return hi;
	}

	void sethi(uint8_t val)
	{
	    hi = val;
	    reg = ((val << 8) | (reg & 0xFF));
	}

	uint8_t getlo()
	{
	    return lo;
	}

	void setlo(uint8_t val)
	{
	    lo = val;
	    reg = ((reg & 0xFF00) | (val));
	}

	uint16_t getreg()
	{
	    return reg;
	}

	void setreg(uint16_t val)
	{
	    reg = val;
	    hi = (val >> 8);
	    lo = (val & 0xFF);
	}
    };



    class LIBMBGB_API CPU
    {
	public:
	    CPU(MMU& memory, GPU& graphics, Timers& timers, Serial& serial);
	    ~CPU();

	    CPUState state = CPUState::Running;

	    void init();
	    void initnobios();
	    void initbios();
	    void shutdown();

	    MMU& mem;
	    GPU& gpu;
	    Timers& timer;
	    Serial& link;

	    Register af;
	    Register bc;
	    Register de;
	    Register hl;
	    uint16_t pc;
	    uint16_t sp;

	    int runfor(int cycles);
	    void hardwaretick(int cycles);
	    void haltedtick(int cycles);
	    int executenextopcode(uint8_t opcode);
	    int executenextcbopcode(uint8_t opcode);
	    int handleinterrupts();
	    void serviceinterrupt(uint16_t addr);

	    bool interruptmasterenable = false;
	    bool enableinterruptsdelayed = false;

	    void enabledelayedinterrupts();

	    void printregs();

	    inline void doubleexec()
	    {
		uint8_t key1 = mem.key1;

		if (TestBit(key1, 0))
		{
		    mem.doublespeed = !mem.doublespeed;
		}

		uint8_t doubletemp = (key1 & 0x7E) | (((mem.doublespeed) ? 1 : 0) << 7);
		mem.key1 = doubletemp;
		pc++;
	    }

	    uint8_t getimmbyte()
	    {
		uint8_t temp = mem.readByte(pc++);
		hardwaretick(4);
		return temp;
	    }

	    int8_t getimmsignedbyte()
	    {
		int8_t temp = (int8_t)(mem.readByte(pc++));
		hardwaretick(4);
		return temp;
	    }

	    uint16_t getimmword()
	    {
		hardwaretick(4);
		uint16_t temp = mem.readWord(pc);
		hardwaretick(4);

		pc += 2;

		return temp;
	    }

	    inline void setcarryflag()
	    {
		setcarry(true);
		setsubtract(false);
		sethalf(false);
	    }

	    inline void compcarryflag()
	    {
		setcarry(!iscarry());
		setsubtract(false);
		sethalf(false);
	    }

	    void load8intomem(uint16_t reg, uint8_t imm)
	    {
		mem.writeByte(reg, imm);
		hardwaretick(4);
	    }

	    uint8_t store8frommem(uint16_t reg)
	    {
		uint8_t temp = mem.readByte(reg);
		hardwaretick(4);
		return temp;
	    }

	    void load16intomem(uint16_t reg, uint16_t imm)
	    {
		mem.writeWord(reg, imm);
		hardwaretick(8);
	    }

	    inline uint16_t loadspn(int8_t val)
	    {
		uint8_t temp = (uint8_t)(val);		

		setzero(false);
		setsubtract(false);
		sethalf(((sp & 0x000F) + (temp & 0x0F)) & 0x0010);
		setcarry(((sp & 0x00FF) + (temp)) & 0x0100);

		hardwaretick(4);
		return (sp + val);
	    }

	    inline void compareg()
	    {
		af.sethi(~af.gethi());
		setsubtract(true);
		sethalf(true);
	    }

	    inline void daa()
	    {
		uint8_t temp = af.gethi();

		if (issubtract())
		{
		    if (iscarry())
		    {
			temp -= 0x60;
		    }

		    if (ishalf())
		    {
			temp -= 0x06;
		    }
		}
		else
		{
		    if (iscarry() || af.gethi() > 0x99)
		    {
			temp += 0x60;
			setcarry(true);
		    }

		    if (ishalf() || (af.gethi() & 0x0F) > 0x09)
		    {
			temp += 0x06;
		    }
		}

		setzero(temp == 0);
		sethalf(false);

		af.sethi(temp);
	    }

	    inline uint8_t addreg(uint8_t reg)
	    {
		uint8_t temp = (af.gethi() + reg);
		setzero(temp == 0);
		setsubtract(false);
		sethalf(((af.gethi() & 0x0F) + (reg & 0x0F)) > 0x0F);
		setcarry((af.gethi() + reg) > 0xFF);
		return temp;
	    }

	    inline uint8_t adcreg(uint8_t reg)
	    {
		uint16_t carrybit = (iscarry() ? 1 : 0);
		uint16_t temp = (af.gethi() + reg + carrybit);
		setzero((temp & 0xFF) == 0);
		setsubtract(false);
		sethalf(((af.gethi() & 0x0F) + (reg & 0x0F) + carrybit) > 0x0F);
		setcarry(temp > 0xFF);
		return temp;
	    }

	    inline uint16_t add16reg(uint16_t reg1, uint16_t reg2)
	    {
		uint16_t temp = (reg1 + reg2);
		setsubtract(false);
		sethalf(((reg1 & 0xFFF) + (reg2 & 0xFFF)) & 0x1000);
		setcarry((reg1 + reg2) & 0x10000);
		hardwaretick(4);
		
		return temp;
	    }

	    inline void addsp(int8_t val)
	    {
		setzero(false);
		setsubtract(false);

		uint8_t temp = (uint8_t)(val);

		sethalf(((sp & 0x000F) + (temp & 0x0F)) & 0x0010);
		setcarry(((sp & 0x00FF) + (temp)) & 0x0100);

		sp += val;
		hardwaretick(8);
	    }

	    inline uint8_t subreg(uint8_t reg)
	    {
		uint8_t temp = (af.gethi() - reg);
		setzero(temp == 0);
		setsubtract(true);
		sethalf(((af.gethi() & 0x0F) - (reg & 0x0F)) < 0);
		setcarry(af.gethi() < reg);
		return temp;
	    }

	    inline uint8_t sbcreg(uint8_t reg)
	    {
		uint8_t carrybit = (iscarry() ? 1 : 0);
		uint8_t temp = (af.gethi() - (reg + carrybit));
		setzero(temp == 0);
		setsubtract(true);
		sethalf(((af.gethi() & 0x0F) < (reg & 0x0F) + carrybit));
		setcarry(af.gethi() < (reg + carrybit));
		return temp;
	    }

	    inline void andreg(uint8_t reg)
	    {
		af.sethi(af.gethi() & reg);
		setzero(af.gethi() == 0);
		setsubtract(false);
		sethalf(true);
		setcarry(false);
	    }

	    inline void orreg(uint8_t reg)
	    {
		af.sethi(af.gethi() | reg);
		setzero(af.gethi() == 0);
		setsubtract(false);
		sethalf(false);
		setcarry(false);
	    }

	    inline void xorreg(uint8_t reg)
	    {
		af.sethi(af.gethi() ^ reg);
		setzero(af.gethi() == 0);
		setsubtract(false);
		sethalf(false);
		setcarry(false);
	    }

	    inline void cmpreg(uint8_t reg)
	    {
		uint8_t temp = (af.gethi() - reg);
		setzero(temp == 0);
		setsubtract(true);
		sethalf(((af.gethi() & 0x0F) - (reg & 0x0F)) < 0);
		setcarry(af.gethi() < reg);
	    }

	    inline uint8_t incregbyte(uint8_t reg)
	    {
		uint8_t temp = reg;
		sethalf((reg & 0x0F) == 0x0F);
		temp += 1;
		setzero((temp == 0));
		setsubtract(false);
		return temp;
	    }

	    inline uint8_t decregbyte(uint8_t reg)
	    {
		uint8_t temp = reg;
		sethalf((reg & 0x0F) == 0x00);
		temp -= 1;
		setzero((temp == 0));
		setsubtract(true);
		return temp;
	    }

	    inline uint16_t increg(uint16_t reg)
	    {
		return (reg + 1);
	    }

	    inline uint16_t decreg(uint16_t reg)
	    {
		return (reg - 1);
	    }

	    inline void jump(uint16_t addr)
	    {
		hardwaretick(4);
		pc = addr;
	    }

	    inline int jumpcond(uint16_t addr, bool cond)
	    {
		int temp = 0;
		if (cond)
		{
		    jump(addr);
		    temp = 16;
		}
		else
		{
		    hardwaretick(8);
		    temp = 12;
		}

		return temp;
	    }

	    inline void reljump(int8_t imm)
	    {
		hardwaretick(4);
		pc += imm;
	    }

	    inline int reljumpcond(int8_t imm, bool cond)
	    {
		int temp = 0;		

		if (cond)
		{
		    reljump(imm);
		    temp = 12;
		}
		else
		{
		    hardwaretick(4);
		    temp = 8;
		}

		return temp;
	    }

	    inline void call(uint16_t addr)
	    {
		hardwaretick(4);
		sp -= 2;
		hardwaretick(4);
		mem.writeWord(sp, pc);
		hardwaretick(4);

		pc = addr;
	    }

	    inline int callcond(uint16_t addr, bool cond)
	    {
		int temp = 0;

		if (cond)
		{
		    call(addr);
		    temp = 24;
		}
		else
		{
		    hardwaretick(8);
		    temp = 12;
		}

		return temp;
	    }

	    inline void ret()
	    {
		uint16_t temp = 0;
		hardwaretick(4);
		temp = mem.readWord(sp);
		hardwaretick(4);
		sp += 2;
		hardwaretick(4);
		pc = temp;
	    }

	    inline int retcond(bool cond)
	    {
		int temp = 0;

		hardwaretick(4);
		
		if (cond)
		{
		    ret();
		    temp = 20;
		}
		else
		{
		    temp = 8;
		}

		return temp;
	    }

	    inline void push(uint16_t reg)
	    {
		hardwaretick(4);
		sp -= 2;
		hardwaretick(4);
		mem.writeWord(sp, reg);
		hardwaretick(4);
	    }

	    inline uint16_t pop()
	    {
		uint16_t temp = 0;
		hardwaretick(4);
		temp = mem.readWord(sp);
		hardwaretick(4);
		sp += 2;

		return temp;
	    }

	    inline uint8_t rl(uint8_t reg)
	    {
		uint8_t temp = reg;
		bool carryval = TestBit(reg, 7);

		temp = ((reg << 1) | (iscarry() ? 1 : 0));

		setzero(temp == 0);

		setsubtract(false);
		sethalf(false);
		setcarry(carryval);

		return temp;
	    }

	    inline uint8_t rlc(uint8_t reg)
	    {
		uint8_t temp = reg;

		setcarry(TestBit(reg, 7));

		temp = ((reg << 1) | (reg >> 7));

		setzero(temp == 0);

		setsubtract(false);
		sethalf(false);

		return temp;
	    }

	    inline uint8_t rr(uint8_t reg)
	    {
		uint8_t temp = reg;
		bool carryval = TestBit(reg, 0);

		temp = ((reg >> 1) | ((iscarry() ? 1 : 0) << 7));

		setzero(temp == 0);

		setsubtract(false);
		sethalf(false);
		setcarry(carryval);

		return temp;
	    }

	    inline uint8_t rrc(uint8_t reg)
	    {
		uint8_t temp = reg;

		setcarry(TestBit(reg, 0));

		temp = ((reg >> 1) | (reg << 7));

		setzero(temp == 0);

		setsubtract(false);
		sethalf(false);

		return temp;
	    }

	    inline uint8_t sla(uint8_t reg)
	    {
		setcarry(TestBit(reg, 7));

		uint8_t temp = (reg << 1);

		setzero(temp == 0);
		setsubtract(false);
		sethalf(false);

		return temp;
	    }

	    inline uint8_t sra(uint8_t reg)
	    {
		setcarry(TestBit(reg, 0));

		uint8_t temp = (reg >> 1);
		temp |= ((temp & 0x40) << 1);

		setzero(temp == 0);
		setsubtract(false);
		sethalf(false);

		return temp;
	    }

	    inline uint8_t srl(uint8_t reg)
	    {
		setcarry(TestBit(reg, 0));

		uint8_t temp = (reg >> 1);

		setzero(temp == 0);
		setsubtract(false);
		sethalf(false);

		return temp;
	    }

	    inline uint8_t swap(uint8_t reg)
	    {
		uint8_t temp = ((reg << 4) | (reg >> 4));
		setzero(temp == 0);
		setsubtract(false);
		sethalf(false);
		setcarry(false);
		return temp;
	    }

	    inline void bit(uint8_t reg, int bit)
	    {
		setzero(!TestBit(reg, bit));
		setsubtract(false);
		sethalf(true);
	    }

	    inline uint8_t res(uint8_t reg, int bit)
	    {
		return BitReset(reg, bit);
	    }

	    inline uint8_t set(uint8_t reg, int bit)
	    {
		return BitSet(reg, bit);
	    }

	    inline void halt()
	    {
		if (!interruptmasterenable && mem.requestedenabledinterrupts())
		{
		    state = CPUState::HaltBug;
		}
		else
		{
		    state = CPUState::Halted;
		}
	    }

	    inline void stop()
	    {
		++pc;
		haltedtick(4);

		state = CPUState::Stopped;
	    }

	    inline void stoppedtick()
	    {
		haltedtick(4);

		state = CPUState::Running;
	    }

	    void setzero(bool val)
	    {
		uint8_t temp = af.getlo();
		temp = (val) ? BitSet(temp, 7) : BitReset(temp, 7);
		af.setlo(temp);
	    }

	    bool iszero()
	    {
		return TestBit(af.getlo(), 7);
	    }

	    void setsubtract(bool val)
	    {
		uint8_t temp = af.getlo();
		temp = (val) ? BitSet(temp, 6) : BitReset(temp, 6);
		af.setlo(temp);
	    }

	    bool issubtract()
	    {
		return TestBit(af.getlo(), 6);
	    }

	    void sethalf(bool val)
	    {
		uint8_t temp = af.getlo();
		temp = (val) ? BitSet(temp, 5) : BitReset(temp, 5);
		af.setlo(temp);
	    }

	    bool ishalf()
	    {
		return TestBit(af.getlo(), 5);
	    }

	    void setcarry(bool val)
	    {
		uint8_t temp = af.getlo();
		temp = (val) ? BitSet(temp, 4) : BitReset(temp, 4);
		af.setlo(temp);
	    }

	    bool iscarry()
	    {
		return TestBit(af.getlo(), 4);
	    }
	
    };
};


#endif // LIBMBGB_CPU
