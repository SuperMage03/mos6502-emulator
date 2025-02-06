#ifndef _MOS6502_HPP_
#define _MOS6502_HPP_
// Standard Library Headers
#include <cstdint>
#include <ostream>
#include <array>
#include <variant>

#define MOS6502_NUMBER_OF_INSTRUCTIONS 256
#define MOS6502_CLOCK_SPEED 1.789773 // In MHz
#define MOS6502_CLOCK_PERIOD 558.73007 // In nanoseconds per cycle

// Forward Delares BUS class
class BUS;

class MOS6502 {
public:
    struct Instruction {
        const std::string name;
        void (*addressingMode)(MOS6502& cpu);
        void (*operationFn)(MOS6502& cpu);
        uint8_t cycles;
    };

    // Usage: Maps OPCODE to Instruction
    static const std::array<Instruction, MOS6502_NUMBER_OF_INSTRUCTIONS> instruction_lookup_table;

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
        const std::variant<uint16_t, Register>& get() const;
        void operator=(const uint16_t& virtual_address);
        void operator=(const Register& target_register);
        uint8_t& operator*() const;
        Pointer& operator++();
        Pointer& operator+=(const int16_t& increment);
        friend class MOS6502;
    };

    enum StatusFlag {
        CARRY = 0,
        ZERO,
        INTERRUPT_DISABLE,
        DECIMAL_MODE,
        BREAK,
        OVERFLOW_FLAG,
        NEGATIVE
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
    * @brief  Gets the value of the given processor status flag
    * @param  flag: status flag to get value from
    * @return State of the given processor status flag
    */
    uint8_t getStatusFlag(const StatusFlag& flag) const;

    /**
    * @brief  Sets the value of the given processor status flag
    * @param  flag: status flag to set value to
    * @param  value: new status flag value
    * @return None
    */
    void setStatusFlag(const StatusFlag& flag, const uint16_t& value);

    /**
    * @brief  Pops 1 byte from stack
    * @param  None
    * @return None
    */
    uint8_t stackPop();

    /**
    * @brief  Pushes 1 byte to stack
    * @param  data: data to be pushed to stack
    * @return None
    */
    void stackPush(const uint8_t& data);

    /**
    * @brief  Executes ADC Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void ADC(MOS6502& cpu);

    /**
    * @brief  Executes AND Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void AND(MOS6502& cpu);

    /**
    * @brief  Executes ASL Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void ASL(MOS6502& cpu);

    /**
    * @brief  Executes BCC Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void BCC(MOS6502& cpu);

    /**
    * @brief  Executes BCS Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void BCS(MOS6502& cpu);

    /**
    * @brief  Executes BEQ Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void BEQ(MOS6502& cpu);

    /**
    * @brief  Executes BIT Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void BIT(MOS6502& cpu);

    /**
    * @brief  Executes BMI Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void BMI(MOS6502& cpu);

    /**
    * @brief  Executes BNE Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void BNE(MOS6502& cpu);

    /**
    * @brief  Executes BPL Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void BPL(MOS6502& cpu);

    /**
    * @brief  Executes BRK Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void BRK(MOS6502& cpu);

    /**
    * @brief  Executes BVC Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void BVC(MOS6502& cpu);

    /**
    * @brief  Executes BVS Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void BVS(MOS6502& cpu);

    /**
    * @brief  Executes CLC Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void CLC(MOS6502& cpu);

    /**
    * @brief  Executes CLD Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void CLD(MOS6502& cpu);

    /**
    * @brief  Executes CLI Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void CLI(MOS6502& cpu);

    /**
    * @brief  Executes CLV Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void CLV(MOS6502& cpu);

    /**
    * @brief  Executes CMP Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void CMP(MOS6502& cpu);

    /**
    * @brief  Executes CPX Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void CPX(MOS6502& cpu);

    /**
    * @brief  Executes CPY Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void CPY(MOS6502& cpu);

    /**
    * @brief  Executes DEC Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void DEC(MOS6502& cpu);
    
    /**
    * @brief  Executes DEX Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void DEX(MOS6502& cpu);
    
    /**
    * @brief  Executes DEY Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void DEY(MOS6502& cpu);
    
    /**
    * @brief  Executes EOR Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void EOR(MOS6502& cpu);
    
    /**
    * @brief  Executes INC Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void INC(MOS6502& cpu);
    
    /**
    * @brief  Executes INX Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void INX(MOS6502& cpu);
    
    /**
    * @brief  Executes INY Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void INY(MOS6502& cpu);
    
    /**
    * @brief  Executes JMP Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void JMP(MOS6502& cpu);
    
    /**
    * @brief  Executes JSR Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void JSR(MOS6502& cpu);
    
    /**
    * @brief  Executes LDA Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void LDA(MOS6502& cpu);
    
    /**
    * @brief  Executes LDX Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void LDX(MOS6502& cpu);
    
    /**
    * @brief  Executes LDY Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void LDY(MOS6502& cpu);
    
    /**
    * @brief  Executes LSR Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void LSR(MOS6502& cpu);
    
    /**
    * @brief  Executes NOP Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void NOP(MOS6502& cpu);
    
    /**
    * @brief  Executes ORA Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void ORA(MOS6502& cpu);
    
    /**
    * @brief  Executes PHA Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void PHA(MOS6502& cpu);
    
    /**
    * @brief  Executes PHA Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void PHP(MOS6502& cpu);
    
    /**
    * @brief  Executes PLA Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void PLA(MOS6502& cpu);
    
    /**
    * @brief  Executes PLP Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void PLP(MOS6502& cpu);
    
    /**
    * @brief  Executes ROL Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void ROL(MOS6502& cpu);

    /**
    * @brief  Executes ROR Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void ROR(MOS6502& cpu);
    
    /**
    * @brief  Executes RTI Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void RTI(MOS6502& cpu);
    
    /**
    * @brief  Executes RTS Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void RTS(MOS6502& cpu);
    
    /**
    * @brief  Executes SBC Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void SBC(MOS6502& cpu);
    
    /**
    * @brief  Executes SEC Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void SEC(MOS6502& cpu);
    
    /**
    * @brief  Executes SED Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void SED(MOS6502& cpu);
    
    /**
    * @brief  Executes SEI Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void SEI(MOS6502& cpu);
    
    /**
    * @brief  Executes STA Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void STA(MOS6502& cpu);
    
    /**
    * @brief  Executes STX Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void STX(MOS6502& cpu);
    
    /**
    * @brief  Executes STY Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void STY(MOS6502& cpu);
    
    /**
    * @brief  Executes TAX Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void TAX(MOS6502& cpu);
    
    /**
    * @brief  Executes TAY Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void TAY(MOS6502& cpu);
    
    /**
    * @brief  Executes TSX Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void TSX(MOS6502& cpu);
    
    /**
    * @brief  Executes TXA Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void TXA(MOS6502& cpu);
    
    /**
    * @brief  Executes TXS Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void TXS(MOS6502& cpu);

    /**
    * @brief  Executes TYA Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void TYA(MOS6502& cpu);

    /**
    * @brief  Executes Unofficial Instruction
    * @param  cpu: Target CPU
    * @return None
    */
    static void XXX(MOS6502& cpu);

    /**
    * @brief  Populate Emulated Data Path Variables Using Implicit Addressing Mode
    * @param  cpu: Target CPU
    * @return None
    */
    static void IMP(MOS6502& cpu);

    /**
    * @brief  Populate Emulated Data Path Variables Using Immediate Addressing Mode
    * @param  cpu: Target CPU
    * @return None
    */
    static void IMM(MOS6502& cpu);

    /**
    * @brief  Populate Emulated Data Path Variables Using Zero Page Addressing Mode
    * @param  cpu: Target CPU
    * @return None
    */
    static void ZP0(MOS6502& cpu);

    /**
    * @brief  Populate Emulated Data Path Variables Using Zero Page X Addressing Mode
    * @param  cpu: Target CPU
    * @return None
    */
    static void ZPX(MOS6502& cpu);

    /**
    * @brief  Populate Emulated Data Path Variables Using Zero Page Y Addressing Mode
    * @param  cpu: Target CPU
    * @return None
    */
    static void ZPY(MOS6502& cpu);

    /**
    * @brief  Populate Emulated Data Path Variables Using Relative Addressing Mode
    * @param  cpu: Target CPU
    * @return None
    */
    static void REL(MOS6502& cpu);


    /**
    * @brief  Populate Emulated Data Path Variables Using Absolute Addressing Mode
    * @param  cpu: Target CPU
    * @return None
    */
    static void ABS(MOS6502& cpu);


    /**
    * @brief  Populate Emulated Data Path Variables Using Absolute X Addressing Mode
    * @param  cpu: Target CPU
    * @return None
    */
    static void ABX(MOS6502& cpu);

    /**
    * @brief  Populate Emulated Data Path Variables Using Absolute Y Addressing Mode
    * @param  cpu: Target CPU
    * @return None
    */
    static void ABY(MOS6502& cpu);

    /**
    * @brief  Populate Emulated Data Path Variables Using Indirect Addressing Mode
    * @param  cpu: Target CPU
    * @return None
    */
    static void IND(MOS6502& cpu);

    /**
    * @brief  Populate Emulated Data Path Variables Using Indirect X Addressing Mode
    * @param  cpu: Target CPU
    * @return None
    */
    static void IZX(MOS6502& cpu);

    /**
    * @brief  Populate Emulated Data Path Variables Using Indirect Y Addressing Mode
    * @param  cpu: Target CPU
    * @return None
    */
    static void IZY(MOS6502& cpu);
};

#endif
