#ifndef _MOS6502_HPP_
#define _MOS6502_HPP_

#include <cstdint>
#include <ostream>
#include <unordered_map>

#define MOS6502_CLOCK_SPEED 1.789773 // In MHz
#define MOS6502_CLOCK_PERIOD 558.73007 // In nanoseconds per cycle

class MOS6502 {
public:
    struct Instruction {
        void (*addressingMode)(MOS6502& cpu);
        void (*operationFn)(MOS6502& cpu);
        uint8_t cycles;
    };

    enum FLAG {
        CARRY = 0,
        ZERO,
        INTERRUPT_DISABLE,
        DECIMAL_MODE,
        BREAK,
        OVERFLOW_FLAG,
        NEGATIVE
    };

private:
    // MOS6502 Registers
    uint16_t program_counter_;
    uint8_t stack_ptr_;
    uint8_t accumulator_;
    uint8_t x_reg_;
    uint8_t y_reg_;

    // Emulator Variables
    uint64_t total_cycle_ran_;

    // Variables needed for fetch->decode->execute cycle
    const Instruction* instruction_; // Current fetched instruction
    uint8_t instruction_opcode_; // Current fetched instruction's opcode
    uint8_t instruction_cycle_remaining_; // Cycles remaining for the current instruction to complete
    
    // Variables that emulates the data carried on a data-path
    uint8_t instruction_operand_data_;
    uint16_t data_memory_unit_address_data_;
    int8_t relative_addressing_data_;
    uint8_t* instruction_operand_physical_memory_address_;

    union {
        uint8_t RAW_VALUE;
        struct {
            uint8_t CARRY : 1;
            uint8_t ZERO : 1;
            uint8_t INTERRUPT_DISABLE : 1;
            uint8_t DECIMAL_MODE : 1;
            uint8_t BREAK : 1;
            uint8_t UNUSED : 1;
            uint8_t OVERFLOW_FLAG : 1;
            uint8_t NEGATIVE : 1;
        };
    } processor_status_;

    /**
    * @brief  Executes ADC Instruction (Accumulator = Accumulator + fetched operand + Carry)
    * @param  cpu: Target CPU
    * @return None
    */
    static void ADC(MOS6502& cpu);

    /**
    * @brief  Executes AND Instruction (Accumulator = Accumulator & fetched operand)
    * @param  cpu: Target CPU
    * @return None
    */
    static void AND(MOS6502& cpu);

    /**
    * @brief  Executes ASL Instruction (Accumulator = fetched operand << 1)
    * @param  cpu: Target CPU
    * @return None
    */
    static void ASL(MOS6502& cpu);

    /**
    * @brief  Executes BCC Instruction (PC +=  if Carry Status Flag is 0)
    * @param  cpu: Target CPU
    * @return None
    */
    static void BCC(MOS6502& cpu);

    /**
    * @brief  Executes BCS Instruction (PC = operand if Carry Status Flag is 1)
    * @param  cpu: Target CPU
    * @return None
    */
    static void BCS(MOS6502& cpu);

    /**
    * @brief  Executes BEQ Instruction (PC = operand if Zero Status Flag is 1)
    * @param  cpu: Target CPU
    * @return None
    */
    static void BEQ(MOS6502& cpu);

    /**
    * @brief  Populate Emulated Data Path Variables Using Implicit Addressing Mode
    * @param  cpu: Target CPU
    * @return None
    */
    static void ImplicitAddressingMode(MOS6502& cpu);

    /**
    * @brief  Populate Emulated Data Path Variables Using Immediate Addressing Mode
    * @param  cpu: Target CPU
    * @return None
    */
    static void ImmediateAddressingMode(MOS6502& cpu);

    /**
    * @brief  Populate Emulated Data Path Variables Using Zero Page Addressing Mode
    * @param  cpu: Target CPU
    * @return None
    */
    static void ZeroPageAddressingMode(MOS6502& cpu);

    /**
    * @brief  Populate Emulated Data Path Variables Using Zero Page X Addressing Mode
    * @param  cpu: Target CPU
    * @return None
    */
    static void ZeroPageXAddressingMode(MOS6502& cpu);

    /**
    * @brief  Populate Emulated Data Path Variables Using Zero Page Y Addressing Mode
    * @param  cpu: Target CPU
    * @return None
    */
    static void ZeroPageYAddressingMode(MOS6502& cpu);

    /**
    * @brief  Populate Emulated Data Path Variables Using Relative Addressing Mode
    * @param  cpu: Target CPU
    * @return None
    */
    static void RelativeAddressingMode(MOS6502& cpu);


    /**
    * @brief  Populate Emulated Data Path Variables Using Absolute Addressing Mode
    * @param  cpu: Target CPU
    * @return None
    */
    static void AbsoluteAddressingMode(MOS6502& cpu);


    /**
    * @brief  Populate Emulated Data Path Variables Using Absolute X Addressing Mode
    * @param  cpu: Target CPU
    * @return None
    */
    static void AbsoluteXAddressingMode(MOS6502& cpu);

    /**
    * @brief  Populate Emulated Data Path Variables Using Absolute Y Addressing Mode
    * @param  cpu: Target CPU
    * @return None
    */
    static void AbsoluteYAddressingMode(MOS6502& cpu);

public:
    // Usage: Maps OPCODE to Instruction
    static const std::unordered_map<uint8_t, Instruction> instruction_lookup_table;

    MOS6502();

    /**
    * @brief  Run 1 cycle of the CPU
    * @param  None
    * @return None
    */
    void runCycle();

    /**
    * @brief  Output the current CPU state
    * @param  out: The output stream
    * @return None
    */
    void outputCurrentState(std::ostream &out) const;

    /**
    * @brief  Reads 1 byte of data at given memory address
    * @param  address: The memory address to read
    * @return Data read at address
    */
    uint8_t readMemory(const uint16_t& address) const;

    /**
    * @brief  Writes 1 byte of data at given memory address
    * @param  address: The memory address to write
    * @param  data: Data to write
    * @return True if successfully written, false otherwise
    */
    bool writeMemory(const uint16_t& address, const uint8_t& data);

    /**
    * @brief  returns the physical memory address of the virtual memory address
    * @param  virtual_address: The virtual memory address
    * @return Physical memory address corresponding to the virtual memory address
    */
    uint8_t* getPhysicalMemoryAddress(const uint16_t& virtual_address);
};

#endif
