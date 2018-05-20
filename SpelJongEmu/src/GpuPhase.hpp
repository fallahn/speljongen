#pragma once

#include <cstdint>

class GpuPhase
{
public:
    virtual ~GpuPhase() = default;

    virtual bool tick() = 0;
};

class HBlankPhase final : public GpuPhase
{
public:

    void start(std::uint32_t tickCount) { m_ticks = tickCount; }

    bool tick() override { return ++m_ticks < 456; }

private:
    std::uint32_t m_ticks = 0;
};

class VBlankPhase final : public GpuPhase
{
public:

    void start() { m_ticks = 0; }

    bool tick() override { return ++m_ticks < 456; }

private:
    std::uint32_t m_ticks = 0;
};