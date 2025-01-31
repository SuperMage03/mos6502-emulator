#ifndef _MOS6502_HPP_
#define _MOS6502_HPP_
// Standard Library Headers
#include <cstdint>
#include <ostream>
#include <unordered_map>
#include <variant>

#define MOS6502_CLOCK_SPEED 1.789773 // In MHz
#define MOS6502_CLOCK_PERIOD 558.73007 // In nanoseconds per cycle

// Forward Delares BUS class
class BUS;

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
    // Class for mos6502 address pointer using similar idea as an iterator
    class Pointer {
    public:
        enum class Register {
            ACCUMULATOR,
        };
    private:
        MOS6502& cpu_;
        std::variant<uint16_t, Register> location_to_point;
        explicit Pointer(MOS6502& cpu, const uint16_t& virtual_address);
        explicit Pointer(MOS6502& cpu, const Register& target_register);
    public:
        void operator=(const uint16_t& virtual_address);
        void operator=(const Register& target_register);
        uint8_t& operator*() const;
        Pointer& operator++();
        Pointer& operator+=(const int16_t& increment);
        friend class MOS6502;
    };

    BUS* bus;

    // MOS6502 Registers
    uint16_t program_counter_;
    uint8_t stack_ptr_;
    uint8_t accumulator_;
    uint8_t x_reg_;
    uint8_t y_reg_;

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

    // Emulator Variables
    uint64_t total_cycle_ran_;

    // Variables needed for fetch->decode->execute cycle
    const Instruction* instruction_; // Current fetched instruction
    uint8_t instruction_opcode_; // Current fetched instruction's opcode
    uint8_t instruction_cycle_remaining_; // Cycles remaining for the current instruction to complete
    
    // Variables that emulates the data carried on a data-path
    Pointer operand_address_;
    int8_t relative_addressing_offset_;

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

    /**
    * @brief  Populate Emulated Data Path Variables Using Indirect Addressing Mode
    * @param  cpu: Target CPU
    * @return None
    */
    static void IndirectAddressingMode(MOS6502& cpu);

    /**
    * @brief  Populate Emulated Data Path Variables Using Indirect X Addressing Mode
    * @param  cpu: Target CPU
    * @return None
    */
    static void IndirectXAddressingMode(MOS6502& cpu);

    /**
    * @brief  Populate Emulated Data Path Variables Using Indirect Y Addressing Mode
    * @param  cpu: Target CPU
    * @return None
    */
    static void IndirectYAddressingMode(MOS6502& cpu);

public:
    // Usage: Maps OPCODE to Instruction
    static const std::unordered_map<uint8_t, Instruction> instruction_lookup_table;

    MOS6502();

    /**
    * @brief  Connects CPU to BUS
    * @param  None
    * @return None
    */
    void connectBUS(BUS* target_bus);

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
    * @brief  returns a reference of the memory at the virtual memory address
    * @param  virtual_address: The virtual memory address
    * @return a reference of the memory at the virtual memory address
    */
    uint8_t& getReferenceToMemory(const uint16_t& virtual_address);
};

#endif
