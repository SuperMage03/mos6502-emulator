#include "bus.hpp"

BUS::BUS(MOS6502& cpu, MemoryUnit& ram): cpu_(cpu), ram_(ram) {
    cpu_.connectBUS(this);
}

uint8_t BUS::readBusData(const uint16_t& address) const {
    return ram_.read(address);
}

bool BUS::writeBusData(const uint16_t& address, const uint8_t& data) {
    return ram_.write(address, data);
}
