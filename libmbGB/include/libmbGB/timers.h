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

#ifndef LIBMBGB_TIMERS
#define LIBMBGB_TIMERS

#include "mmu.h"
#include "libmbgb_api.h"
#include <functional>
using namespace gb;
using namespace std;

namespace gb
{
    class LIBMBGB_API Timers
    {
	public:
	    Timers(MMU& memory);
	    ~Timers();

	    MMU& timermem;

	    void init();
	    void shutdown();

	    void updatetimer();

	    bool prevtimainc = false;
	    bool timaoverflow = false;
	    bool timaoverflownotinterrupted = false;
	    uint8_t prevtimaval = 0x00;

	    uint16_t divbit[4] = {0x0200, 0x0008, 0x0020, 0x0080};

	    inline int tacfreq()
	    {
		return (timermem.readByte(0xFF07) & 0x3);
	    }

	    inline int tacenable()
	    {
		return TestBit(timermem.readByte(0xFF07), 2);
	    }

	    inline bool timaincwentlow(bool timainc)
	    {
		return (!timainc && prevtimainc);
	    }

	    inline bool timawasnotwritten(uint8_t currenttimaval)
	    {
		return (prevtimaval == currenttimaval);
	    }

	    inline void loadtmaintotima()
	    {
		timermem.writeByte(0xFF05, timermem.readByte(0xFF06));
	    }
    };
};

#endif // LIBMBGB_TIMERS
