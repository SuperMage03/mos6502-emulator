#include "ram.hpp"

RAM::RAM(const uint32_t& byte_size): byte_size_(byte_size), memory_block_(std::make_unique<uint8_t[]>(byte_size)) {
    for (uint32_t i = 0; i < byte_size_; i++) {
        memory_block_[i] = 0x00;
    }
}

RAM::RAM(std::ifstream& file_in) {
    file_in.seekg(0, std::ios::end);
    std::streamsize file_size = file_in.tellg();
    file_in.seekg(0, std::ios::beg);

    byte_size_ = file_size;
    memory_block_ = std::make_unique<uint8_t[]>(byte_size_);

    for (uint32_t i = 0; i < byte_size_; i++) {
        file_in >> std::noskipws >> memory_block_[i];
    }
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

uint8_t& RAM::getReferenceToMemory(const uint16_t& virtual_address) {
    return memory_block_[virtual_address];
}
