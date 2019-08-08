// This file is part of libmbGB.
// Copyright (C) 2019 Buenia.
//
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

#include "../../include/libmbGB/libmbgb.h"
using namespace gb;
using namespace std;

namespace gb
{
    GBCore::GBCore()
    {
	coremmu = make_unique<MMU>();
	coregpu = make_unique<GPU>(*coremmu);
	coretimers = make_unique<Timers>(*coremmu);
	coreinput = make_unique<Input>(*coremmu);
	coreserial = make_unique<Serial>(*coremmu);
	coreapu = make_unique<APU>(*coremmu);
	corecpu = make_unique<CPU>(*coremmu, *coregpu, *coretimers, *coreserial, *coreapu);
    }

    GBCore::~GBCore()
    {

    }

    void GBCore::preinit()
    {
	coremmu->init();
    }

    void GBCore::init()
    {
	coremmu->resetio();

	if (!coremmu->biosload)
	{
	    coremmu->initio();
	}

	corecpu->init();
	coregpu->init();
	coretimers->init();
	coreinput->init();
	coreserial->init();
	loadbackup();
	cout << "mbGB::Initialized" << endl;
    }

    void GBCore::shutdown()
    {
	coreserial->shutdown();
	coreinput->shutdown();
	coretimers->shutdown();
	coregpu->shutdown();
	corecpu->shutdown();
	savebackup();
	coremmu->shutdown();
	cout << "mbGB::Shutting down..." << endl;
    }

    void GBCore::printusage(char *argv)
    {
	cout << "Usage: " << argv << " ROM [options]" << endl;
	cout << endl;
	cout << "Options:" << endl;
	cout << "-b [FILE], --bios [FILE] \t\t Loads and uses a BIOS file." << endl;
	cout << "--sys-dmg \t\t Plays ROMs in DMG mode." << endl;
	cout << "--sys-gbc \t\t Plays ROMs in GBC mode." << endl;
	cout << "--sys-hybrid \t\t Plays ROMs in hybrid DMG/GBC mode." << endl;
	cout << "--dotrender \t\t Enables the more accurate dot-based renderer." << endl;
	cout << "-h, --help \t\t Displays this help message." << endl;
	cout << endl;
    }

    bool GBCore::biosload()
    {
	return coremmu->biosload;
    }

    bool GBCore::getoptions(int argc, char* argv[])
    {
	if (argc < 2)
	{
	    printusage(argv[0]);
	    return false;
	}

	for (int i = 1; i < argc; i++)
	{
	    if ((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0))
	    {
		printusage(argv[0]);
		return false;
	    }
	}

	romname = argv[1];

	for (int i = 2; i < argc; i++)
	{
	    if ((strcmp(argv[i], "-b") == 0) || (strcmp(argv[i], "--bios") == 0))
	    {
		if ((i + 1) == argc)
		{
		    cout << "Error - No BIOS file in arguments" << endl;
		    return false;
		}
		else
		{
		    coremmu->biosload = true;
		    biosname = argv[i + 1];
		}
	    }

	    if ((strcmp(argv[i], "--sys-dmg") == 0))
	    {
		coremmu->gameboy = Console::DMG;
	    }

	    if ((strcmp(argv[i], "--sys-gbc") == 0))
	    {
		coremmu->gameboy = Console::CGB;
	    }

	    if ((strcmp(argv[i], "--sys-hybrid") == 0))
	    {
		coremmu->hybrid = true;
	    }

	    if ((strcmp(argv[i], "--dotrender") == 0))
	    {
		setdotrender(true);
	    }
	    else
	    {
		setdotrender(false);
	    }
	}

	return true;
    }

    bool GBCore::loadBIOS(string filename)
    {
	return coremmu->loadBIOS(filename);
    }

    bool GBCore::loadROM(string filename)
    {
	return coremmu->loadROM(filename);
    }

    bool GBCore::loadROM(const char *filename, const uint8_t* buffer, int size)
    {
	return coremmu->loadROM(filename, buffer, size);
    }

    bool GBCore::loadbackup()
    {
	stringstream saveram;

	saveram << romname << ".sav";

	return coremmu->loadbackup(saveram.str());
    }

    bool GBCore::savebackup()
    {
	stringstream saveram;

	saveram << romname << ".sav";

	return coremmu->savebackup(saveram.str());
    }

    bool GBCore::loadstate()
    {
	stringstream savestate;

	savestate << romname << ".mbsave";

	string savename = savestate.str();

	bool ret = false;

	if (!corecpu->loadcpu(savename))
	{
	    cout << "mbGB::Save state could not be loaded." << endl;
	    ret = false;
	}

	int offs = corecpu->cpusize();

	if (!coremmu->loadmmu(offs, savename))
	{
	    cout << "mbGB::Save state could not be loaded." << endl;
	    ret = false;
	}

	cout << "mbGB::Save state succesfully loaded." << endl;
	ret = true;
	
	return ret;
    }

    bool GBCore::savestate()
    {
	stringstream savestate;

	savestate << romname << ".mbsave";

	string savename = savestate.str();

	cout << savename << endl;

	bool ret = false;

	if (!corecpu->savecpu(savename))
	{
	    cout << "mbGB::CPU save state could not be written." << endl;
	    ret = false;
	}

	if (!coremmu->savemmu(savename))
	{
	    cout << "mbGB::MMU save state could not be written." << endl;
	    ret = false;
	}

	cout << "mbGB::Save state succesfully written." << endl;
	ret = true;
	
	return ret;
    }

    RGB GBCore::getpixel(int x, int y)
    {
	return coregpu->framebuffer[x + (y * 160)];
    }

    void GBCore::keypressed(Button button)
    {
	coreinput->keypressed(button);
    }

    void GBCore::keyreleased(Button button)
    {
	coreinput->keyreleased(button);
    }

    bool GBCore::dumpvram(string filename)
    {
	fstream file(filename.c_str(), ios::out | ios::binary);

	if (file.is_open())
	{
	    file.seekp(0, ios::beg);
	    file.write((char*)&coremmu->vram[0], 0x4000);
	    file.close();
	    cout << "mbGB::VRAM dumped" << endl;
	    return true;
	}
	else
	{
	    cout << "mbGB::Error - VRAM could not be dumped" << endl;
	    return false;
	}
    }

    bool GBCore::dumpmemory(string filename)
    {
	fstream file(filename.c_str(), ios::out | ios::binary);

	uint8_t memorymap[0x10000];

	for (int i = 0; i < 0x10000; i++)
	{
	    memorymap[i] = coremmu->readByte(i);
	}

	if (file.is_open())
	{
	    file.seekp(0, ios::beg);
	    file.write((char*)&memorymap[0], 0x10000);
	    file.close();
	    cout << "mbGB::Memory dumped" << endl;
	    return true;
	}
	else
	{
	    cout << "mbGB::Error - Memory could not be dumped" << endl;
	    return false;
	}
    }

    void GBCore::setdotrender(bool val)
    {
	coregpu->setdotrender(val);
    }

    void GBCore::runcore()
    {
	overspentcycles = corecpu->runfor((70224 << coremmu->doublespeed) + overspentcycles);
    }

    bool GBCore::initcore()
    {
	preinit();

	if (biosload())
	{
	    if (!loadBIOS(biosname))
	    {
		return false;
	    }
	}
	

	if (!loadROM(romname))
	{
	    return false;
	}

	init();
	return true;
    }

    bool GBCore::initcore(const char *filename, const uint8_t* buffer, int size)
    {
	preinit();	

	if (!loadROM(filename, buffer, size))
	{
	    return false;
	}

	romname = filename;

	init();
	return true;
    }

    void GBCore::setsamplerate(int val)
    {
	coreapu->setsamplerate(val);
    }

    void GBCore::setaudiocallback(apuoutputfunc cb)
    {
	coreapu->setaudiocallback(cb);
    }

    void GBCore::resetcore()
    {
	shutdown();
	initcore();
    }

    void GBCore::resetcoreretro()
    {
	shutdown();
	init();
    }
};
