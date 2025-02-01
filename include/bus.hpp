#ifndef _BUS_HPP_
#define _BUS_HPP_
// Stardard Library Headers
#include <cstdint>
// Project Headers
#include "mos6502.hpp"
#include "ram.hpp"

class BUS {
public:
    BUS(MOS6502& cpu, RAM& wram, RAM& vram);
    uint8_t readBusData(const uint16_t& address) const;
    bool writeBusData(const uint16_t& address, const uint8_t& data);

    /**
    * @brief  returns a reference of the memory at the virtual memory address
    * @param  virtual_address: The virtual memory address
    * @return a reference of the memory at the virtual memory address
    */
    uint8_t& getReferenceToMemory(const uint16_t& virtual_address);
private:
    MOS6502& cpu_;
    RAM& wram_;
    RAM& vram_;
};

#endif
