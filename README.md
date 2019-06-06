<img src="https://github.com/Buenia0/libmbGB/blob/develop/res/libmbgblogo.png" alt="drawing" width="200"/>

[![Build Status](https://travis-ci.org/Buenia0/libmbGB.svg?branch=master)](https://travis-ci.org/Buenia0/libmbGB)

Game Boy (Color) emulation library, sorta

The goal here is to do things right and fast (like melonDS). But, also, you know, to have a fun challenge... :)

## Screenshots (will fix later)

# DMG Screenshots:

<p align="center">
  <img src="https://github.com/Buenia0/libmbGB/blob/develop/res/screens/kdl.bmp" alt="kdl" width="200"/>
  <img src="https://github.com/Buenia0/libmbGB/blob/develop/res/screens/tetris.bmp" alt="tetris" width="200"/>
  <img src="https://github.com/Buenia0/libmbGB/blob/develop/res/screens/sml.bmp" alt="sml" width="200"/>
  <img src="https://github.com/Buenia0/libmbGB/blob/develop/res/screens/poker.bmp" alt="poker" width="200"/>
</p>

<p align="center">
  <img src="https://github.com/Buenia0/libmbGB/blob/develop/res/screens/mm.bmp" alt="mm" width="200"/>
  <img src="https://github.com/Buenia0/libmbGB/blob/develop/res/screens/poker2.bmp" alt="poker2" width="200"/>
  <img src="https://github.com/Buenia0/libmbGB/blob/develop/res/screens/tetris2.bmp" alt="tetris2" width="200"/>
  <img src="https://github.com/Buenia0/libmbGB/blob/develop/res/screens/smlgc.bmp" alt="smlgc" width="200"/>
</p>


## CGB Screenshots

<p align="center">
  <img src="https://github.com/Buenia0/libmbGB/blob/develop/res/screens/mariogolf.bmp" alt="mariogolf" width="200"/>
  <img src="https://github.com/Buenia0/libmbGB/blob/develop/res/screens/tetrisdx.bmp" alt="tetrisdx" width="200"/>
  <img src="https://github.com/Buenia0/libmbGB/blob/develop/res/screens/pokec.bmp" alt="pokec" width="200"/>
  <img src="https://github.com/Buenia0/libmbGB/blob/develop/res/screens/shantae.bmp" alt="shantae" width="200"/>
</p>

## Current project status

At this point, libmbGB is able to run many commercial Game Boy (and Game Boy Color) titles well and (if the SDL2 renderer is built) with sound. In additon, I'm trying to improve the overall accuracy of libmbGB, so don't lose hope yet... ;)

## Building Instructions

## Mac and Linux:

1. Install dependencies (optional step for example SDL2 renderer):

On Linux:

Ubuntu: `sudo apt install libsdl2-dev`
Arch: `pacman -S sdl2`
Fedora: `dnf install SDL2-devel`

On Mac:

`brew install sdl2`

2. Fetch the source:

`git clone --recursive https://github.com/Buenia0/libmbGB.git`

`cd libmbGB`

3. Compile:

`mkdir build && cd build`

`cmake .. -G "Unix Makefiles" -DBUILD_EXAMPLE="<ON/OFF>" -DBUILD_HEADLESS="<ON/OFF>"`

`make -j4`

## Windows:

Requires MSYS2, which can be installed from their [website.](http://www.msys2.org/)

1. Install dependencies:

For 32-bit builds:

`pacman -S mingw-w64-i686-toolchain mingw-w64-i686-cmake "mingw-w64-i686-SDL2" git`

For 64-bit builds:

`pacman -S mingw-w64-x86_64-toolchain mingw-w64-x86_64-cmake "mingw-w64-x86_64-SDL2" git `

The packages in quotation marks are for the example SDL2 renderer.

2. Fetch the source:

`git clone --recursive https://github.com/Buenia0/libmbGB.git`

`cd libmbGB`

3. Compile:

`mkdir build && cd build`

`cmake .. -G "MSYS Makefiles" -DBUILD_EXAMPLE="<ON/OFF>" -DBUILD_HEADLESS="<ON/OFF>"`

`make -j4`

# Plans

## Near-term

Link Cable support
Various bugfixes

## Medium-term

Improving existing GameBoy Color support

## Long-term

Support for GameBoy (and GameBoy Color) addons

# License

<img src="https://www.gnu.org/graphics/gplv3-127x51.png" alt="drawing" width="150"/>

libmbGB is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

# Copyright

(C) 2019 Buenia. This project is not affiliated in any way with Nintendo. Game Boy is a registered trademark of Nintendo Co., Ltd.

For information regarding libmbGB's stance on copyright infringement, see the DISCLAIMER.md file in this repository.
