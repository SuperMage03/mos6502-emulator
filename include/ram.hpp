#ifndef _RAM_HPP_
#define _RAM_HPP_

#include <cstdint>
#include <memory>

class RAM {
private:
    const uint16_t byte_size_;
    std::unique_ptr<uint8_t[]> memory_block_;
public:
    RAM(const uint16_t& byte_size);
    uint8_t read(const uint16_t& address) const;
    bool write(const uint16_t& address, const uint8_t& data);

    /**
    * @brief  returns the physical memory address of the virtual memory address
    * @param  virtual_address: The virtual memory address
    * @return Physical memory address corresponding to the virtual memory address
    */
    uint8_t* getPhysicalMemoryAddress(const uint16_t& virtual_address);
};

#endif
