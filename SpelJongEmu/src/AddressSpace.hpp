#pragma once

#include <cstdint>
#include <vector>
#include <cassert>
#include <string>

class AddressSpace
{
public:

    AddressSpace(std::vector<std::uint8_t>& storage) :m_storage(storage) {}

    virtual ~AddressSpace() = default;

    virtual std::string getLabel() const { return "Label Missing"; }

    virtual bool accepts(std::uint16_t address) const = 0;

    virtual void setByte(std::uint16_t address, std::uint8_t value) = 0;

    virtual std::uint8_t getByte(std::uint16_t address) const = 0;

protected:
    std::vector<std::uint8_t>& getStorage()
    {
        return m_storage;
    }

    const std::vector<std::uint8_t>& getStorage() const
    {
        return m_storage;
    }

    std::uint8_t getStorageValue(std::uint16_t address) const { return m_storage[address]; }

    void setStorageValue(std::uint16_t address, std::uint8_t value) { m_storage[address] = value; }
private:
    std::vector<std::uint8_t>& m_storage;
};