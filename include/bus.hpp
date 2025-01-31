#ifndef _BUS_HPP_
#define _BUS_HPP_

#include <cstdint>
#include <sys/types.h>
#include <unordered_map>

#include "mos6502.hpp"
#include "ram.hpp"

class BUS {
private:
    MOS6502& cpu_;
    RAM& wram_;
    RAM& vram_;
public:
    BUS(MOS6502& cpu, RAM& wram, RAM& vram);
    uint8_t readBusData(const uint16_t& address) const;
    bool writeBusData(const uint16_t& address, const uint8_t& data);
};

#endif
