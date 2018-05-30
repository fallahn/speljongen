#### Speljongen - Gameboy emulator

Started as a port of [Coffee GB](https://github.com/trekawek/coffee-gb/tree/master) from java to C++ for poops and giggles,
while the CPU/GPU emulation remains mostly unchanged the memory emulation has been implemented slightly differently. The interface,
rendering and OS input handling has been all replaced with [SFML](https://sfml-dev.org) and ImGui and implemented from scratch.


##### Building
Speljongen should compile on Windows, macOS and linux with the included CMake file.
There is also a Visual Studio 2017 project included (with dependencies) for Windows
users.

Speljongen depends on SFML and OpenGL libraries, and on linux you will need to install
pkgmanager and gtk+3 for the native file dialogue windows. On macOS the project needs
to link to AppKit and CoreServices for the same reason. The CMake file also supports
creating app bundles on macOS, which requires a valid icon set and XCode 9 command line
tools installed.



MIT License

Copyright (c) 2017 Tomasz Rękawek (Coffee GB)
Copyright (c) 2018 Matt Marchant (Speljongen)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.