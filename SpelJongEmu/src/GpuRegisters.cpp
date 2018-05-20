#include "GpuRegister.hpp"

const std::vector<GpuRegister> GpuRegister::registers = 
{
    GpuRegister(0xff41, Register::RW),
    {0xff42, Register::RW},
    {0xff43, Register::RW},
    {0xff44, Register::R},
    {0xff45, Register::RW},
    {0xff47, Register::RW},
    {0xff48, Register::RW},
    {0xff49, Register::RW},
    {0xff4a, Register::RW},
    {0xff4b, Register::RW},
    {0xff4f, Register::W}
};