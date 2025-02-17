#ifndef _BUS_HPP_
#define _BUS_HPP_
// Stardard Library Headers
#include <cstdint>
// Project Headers
#include "mos6502.hpp"
#include "memory-unit.hpp"

class BUS {
public:
    /**
    * @brief  Constructor for BUS
    * @param  cpu: CPU on the BUS
    * @param  ram: RAM on the BUS
    * @return None
    */
    BUS(MOS6502& cpu, MemoryUnit& ram);

    /**
    * @brief  Reads data from the bus at the address
    * @param  address: The address to read from
    * @return Data read from the bus
    */
    uint8_t readBusData(const uint16_t& address) const;
    
    /**
    * @brief  Writes data to the bus at the address
    * @param  address: The address to write to
    * @param  data: The data to write
    * @return True if successfully written, false otherwise
    */
    bool writeBusData(const uint16_t& address, const uint8_t& data);

private:
    MOS6502& cpu_;
    MemoryUnit& ram_;
};

#endif
