#include "ram.hpp"
#include <cstdint>
#include <algorithm>

RAM::RAM(const uint16_t& byte_size): byte_size_(byte_size), memory_block_(std::make_unique<uint8_t[]>(byte_size)) {
    std::fill(memory_block_.get(), memory_block_.get() + byte_size, 0x00);
}

uint8_t RAM::read(const uint16_t& address) const {
    return memory_block_[address];
}

bool RAM::write(const uint16_t& address, const uint8_t& data) {
    if (address > byte_size_) {
        return false;
    }
    memory_block_[address] = data;
    return true;
}

uint8_t* RAM::getPhysicalMemoryAddress(const uint16_t& virtual_address) {
    return &memory_block_[virtual_address];
}
