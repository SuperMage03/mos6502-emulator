#include "bus.hpp"

BUS::BUS(MOS6502& cpu, RAM& wram, RAM& vram): cpu_(cpu), wram_(wram), vram_(vram) {
    cpu_.connectBUS(this);
}

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

uint8_t& BUS::getReferenceToMemory(const uint16_t& virtual_address) {
    if (virtual_address < 0x2000) {
        return wram_.getReferenceToMemory(virtual_address & ~(0b11 << 11));
    }
    // Returns memory at NULL of WRAM
    return wram_.getReferenceToMemory(0x00);
}
