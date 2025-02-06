#ifndef _RAM_HPP_
#define _RAM_HPP_

#include <cstdint>
#include <fstream>
#include <memory>

class RAM {
public:
    RAM(const uint32_t& byte_size);
    RAM(std::ifstream& file_in);

    uint8_t read(const uint16_t& address) const;
    bool write(const uint16_t& address, const uint8_t& data);

    /**
    * @brief  returns a reference of the memory at the virtual memory address
    * @param  virtual_address: The virtual memory address
    * @return a reference of the memory at the virtual memory address
    */
    uint8_t& getReferenceToMemory(const uint16_t& virtual_address);
private:
    uint32_t byte_size_;
    std::unique_ptr<uint8_t[]> memory_block_;
};

#endif
