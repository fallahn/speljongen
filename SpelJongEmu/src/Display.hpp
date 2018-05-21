#pragma once

#include <cstdint>

class Display final
{
public:
    Display();
    void putPixel(std::uint8_t);
    void requestRefresh();
    void waitForRefresh();
    void enableLCD();
    void disableLCD();
};