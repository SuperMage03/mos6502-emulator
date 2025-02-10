#ifndef _RAM_HPP_
#define _RAM_HPP_

#include <cstdint>
#include <fstream>
#include <memory>

class RAM {
public:
    /**
    * @brief  Constructor for RAM
    * @param  byte_size: The size of the RAM in bytes
    * @return None
    */
    RAM(const uint32_t& byte_size);

    /**
    * @brief  Constructor for RAM
    * @param  file_in: The file stream to read and initialize RAM from
    * @return None
    */
    RAM(std::ifstream& file_in);

    /**
    * @brief  Reads 1 byte of data at given memory address
    * @param  address: The memory address to read
    * @return Data read at address
    */
    uint8_t read(const uint16_t& address) const;

    /**
    * @brief  Writes 1 byte of data at given memory address
    * @param  address: The memory address to write
    * @param  data: The data to write
    * @return True if successfully written, false otherwise
    */
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
