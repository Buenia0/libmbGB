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

#include "../../include/libmbGB/gpu.h"
using namespace gb;
using namespace std;

namespace gb
{
    GPU::GPU(MMU& memory) : gpumem(memory)
    {
	gpumem.setpoweroncallback(bind(&GPU::updatepoweronstate, this, placeholders::_1));
    }

    GPU::~GPU()
    {

    }

    void GPU::init()
    {
	cout << "GPU::Initialized" << endl;
    }

    void GPU::shutdown()
    {
	cout << "GPU::Shutting down..." << endl;
    }

    void GPU::updatelcd()
    {

    }

    void GPU::renderscanline()
    {
	for (int i = 0; i < 160; i++)
	{
	    if (gpumem.isbgenabled())
	    {
		renderbg(i);
	    }
	}
    }

    void GPU::renderbg(int pixel)
    {
	uint16_t tilemap = TestBit(gpumem.lcdc, 3) ? 0x9C00 : 0x9800;
	uint16_t tiledata = TestBit(gpumem.lcdc, 4) ? 0x8000 : 0x8800;
	bool unsig = TestBit(gpumem.lcdc, 4);

	uint8_t ypos = 0;

	ypos = gpumem.scrolly + gpumem.ly;

	uint16_t tilerow = (((uint8_t)(ypos / 8)) * 32);

	uint8_t xpos = pixel + gpumem.scrollx;

	uint16_t tilecol = (xpos / 8);
	int16_t tilenum = 0;

	uint16_t tileaddr = (tilemap + tilerow + tilecol);

	if (unsig)
	{
	    tilenum = (uint8_t)(gpumem.vram[tileaddr - 0x8000]);
	}
	else
	{
	    tilenum = (int8_t)(gpumem.vram[tileaddr - 0x8000]);
	}

	uint16_t tileloc = tiledata;

	if (unsig)
	{
	    tileloc += (uint16_t)(tilenum * 16);
	}
	else
	{
	    tileloc += (int16_t)(((tilenum + 128) * 16));
	}

	uint8_t line = (ypos % 8);

	line *= 2;
	uint8_t data1 = gpumem.readByte((tileloc + line));
	uint8_t data2 = gpumem.readByte((tileloc + line + 1));

	int colorbit = (xpos % 8);
	colorbit -= 7;
	colorbit *= -1;

	int colornum = BitGetVal(data2, colorbit);
	colornum <<= 1;
	colornum |= BitGetVal(data1, colorbit);

	int red = 0;
	int green = 0;
	int blue = 0;

	int color = getdmgcolor(colornum, gpumem.readByte(0xFF47));

	switch (color)
	{
	    case 0: red = green = blue = 0xFF; break;
	    case 1: red = green = blue = 0xCC; break;
	    case 2: red = green = blue = 0x77; break;
	    case 3: red = green = blue = 0x00; break;
	}

	uint8_t scanline = currentscanline;

	int index = (pixel + (scanline * 160));
	framebuffer[index].red = red;
	framebuffer[index].green = green;
	framebuffer[index].blue = blue;
    }
};
