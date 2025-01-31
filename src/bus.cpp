#include "bus.hpp"
#include <cstdint>

BUS::BUS(MOS6502& cpu, RAM& wram, RAM& vram): cpu_(cpu), wram_(wram), vram_(vram) {}

uint8_t BUS::readBusData(const uint16_t& address) const {
    if (address < 0x2000) {
        return wram_.read(address & ~(0b11 << 11));
    }
    return 0;
}

bool BUS::writeBusData(const uint16_t& address, const uint8_t& data) {
    if (address < 0x2000) {
        return wram_.write(address & ~(0b11 << 11), data);
    }
    return false;
}
