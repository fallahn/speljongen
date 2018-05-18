#pragma once

#include <cstdint>
#include <vector>
#include <cassert>

class AddressSpace
{
public:
    enum Type
    {
        VOID = -1,
        Ram,
        Rom,
        Timer,
        Dma,
        ShadowSpace
    };

    virtual ~AddressSpace() = default;

    virtual Type type() const { return VOID; }

    virtual bool accepts(std::uint16_t address) const = 0;

    virtual void setByte(std::uint16_t address, std::uint8_t value) = 0;

    virtual std::uint8_t getByte(std::uint16_t address) const = 0;

    void setStorage(std::vector<std::uint8_t>& storage) { m_storage = &storage; }

protected:
    std::vector<std::uint8_t>& getStorage()
    {
        assert(m_storage);
        return *m_storage;
    }

    const std::vector<std::uint8_t>& getStorage() const
    {
        assert(m_storage);
        return *m_storage;
    }
private:
    std::vector<std::uint8_t>* m_storage = nullptr;
};