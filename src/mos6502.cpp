#include "mos6502.hpp"
#include <cstdint>

const std::unordered_map<uint8_t, MOS6502::Instruction> MOS6502::instruction_lookup_table = {
    {0x69, {MOS6502::ImmediateAddressingMode, MOS6502::ADC, 2}}
};

MOS6502::MOS6502(): program_counter_(0xFFFC), stack_ptr_(0), accumulator_(0), 
                    x_reg_(0), y_reg_(0), processor_status_({.RAW_VALUE=0}) {}

void MOS6502::runCycle() {
    static bool fetched = false;
    static void (*opcode_fn)(uint16_t& program_counter) = nullptr;

    total_cycle_ran_++;

    // Fetch new instruction (Cost 1 cycle)
    if (!fetched) {
        instruction_opcode_ = readMemory(program_counter_);
        program_counter_++;

        instruction_ = &instruction_lookup_table.at(instruction_opcode_);
        
        instruction_->operationFn(*this);

        fetched = true;
        return;
    }

    // If opcode cycle
    if (instruction_cycle_remaining_ == 0) {
        opcode_fn(program_counter_);
        fetched = false;
    }
    instruction_cycle_remaining_--;
}

void MOS6502::outputCurrentState(std::ostream &out) const {
    for (uint8_t i = 0; i < 8; i++) {
        bool cur_bit = processor_status_.RAW_VALUE & (1 << i);
        out << cur_bit;
    }
    out << std::endl;
    // Output total cycles elapsed
    out << "Cycles Elapsed: " << total_cycle_ran_ << std::endl;
}

void MOS6502::ADC(MOS6502& cpu) {
    uint16_t result = static_cast<uint16_t>(cpu.accumulator_) + static_cast<uint16_t>(cpu.instruction_operand_data_) + static_cast<uint16_t>(cpu.processor_status_.CARRY);

    cpu.processor_status_.ZERO = (result == 0);
    cpu.processor_status_.CARRY = (result > 0x00FF);

    // If the result sign was different from accumulator sign and accumulator sign is the same as the operand sign,
    //   then we have an overflow
    if (((cpu.accumulator_ ^ result) & 0x80) && !((cpu.accumulator_ ^ cpu.instruction_operand_data_) & 0x80)) {
        cpu.processor_status_.OVERFLOW_FLAG = 1;
    }
    else {
        cpu.processor_status_.OVERFLOW_FLAG = 0;
    }

    cpu.processor_status_.NEGATIVE = ((result & 0x0080) > 0);
    // Update the accumulator
    cpu.accumulator_ = result & 0x00FF;
}

void MOS6502::AND(MOS6502& cpu) {
    cpu.accumulator_ &= cpu.instruction_operand_data_;
    cpu.processor_status_.ZERO = (cpu.accumulator_ == 0);
    cpu.processor_status_.NEGATIVE = ((cpu.accumulator_ & 0x80) > 0);
}

void MOS6502::ASL(MOS6502& cpu) {
    uint8_t result = cpu.instruction_operand_data_ << 0x01;

    cpu.processor_status_.CARRY = ((cpu.instruction_operand_data_ & 0x80) > 0);
    cpu.processor_status_.ZERO = (result == 0);
    cpu.processor_status_.NEGATIVE = ((result & 0x80) > 0);

    if (cpu.instruction_->addressingMode == ImmediateAddressingMode) {
        cpu.accumulator_ = result;
    }
    else {
        cpu.writeMemory(cpu.data_memory_unit_address_data_, result);
    }
}

void MOS6502::BCC(MOS6502& cpu) {
    if (!cpu.processor_status_.CARRY) {
        uint8_t new_pc_address = cpu.program_counter_ + cpu.relative_addressing_data_;
        // Branch success adds 1 cycle
        cpu.instruction_cycle_remaining_++;
        // If branched to a new page we need to add 1 more cycle
        if ((cpu.program_counter_ & 0xFF00) != (new_pc_address & 0xFF00)) {
            cpu.instruction_cycle_remaining_++;
        }
        cpu.program_counter_ = new_pc_address;
    }
}

void MOS6502::BCS(MOS6502& cpu) {
    if (cpu.processor_status_.CARRY) {
        uint8_t new_pc_address = cpu.program_counter_ + cpu.relative_addressing_data_;
        // Branch success adds 1 cycle
        cpu.instruction_cycle_remaining_++;
        // If branched to a new page we need to add 1 more cycle
        if ((cpu.program_counter_ & 0xFF00) != (new_pc_address & 0xFF00)) {
            cpu.instruction_cycle_remaining_++;
        }
        cpu.program_counter_ = new_pc_address;
    }
}

void MOS6502::BEQ(MOS6502& cpu) {
    if (cpu.processor_status_.ZERO) {
        uint8_t new_pc_address = cpu.program_counter_ + cpu.relative_addressing_data_;
        // Branch success adds 1 cycle
        cpu.instruction_cycle_remaining_++;
        // If branched to a new page we need to add 1 more cycle
        if ((cpu.program_counter_ & 0xFF00) != (new_pc_address & 0xFF00)) {
            cpu.instruction_cycle_remaining_++;
        }
        cpu.program_counter_ = new_pc_address;
    }
}

void MOS6502::ImplicitAddressingMode(MOS6502& cpu) {
    cpu.instruction_operand_data_ = cpu.accumulator_;
}

void MOS6502::ImmediateAddressingMode(MOS6502& cpu) {
    uint8_t immediate = cpu.readMemory(cpu.program_counter_);
    cpu.program_counter_++;
    cpu.instruction_operand_data_ = immediate;
}

void MOS6502::ZeroPageAddressingMode(MOS6502& cpu) {
    uint8_t target_address = cpu.readMemory(cpu.program_counter_);
    cpu.program_counter_++;

    cpu.data_memory_unit_address_data_ = target_address;
    cpu.instruction_operand_data_ = cpu.readMemory(cpu.data_memory_unit_address_data_);
}

void MOS6502::ZeroPageXAddressingMode(MOS6502& cpu) {
    uint8_t target_address = cpu.readMemory(cpu.program_counter_) + cpu.x_reg_;
    cpu.program_counter_++;

    cpu.data_memory_unit_address_data_ = target_address;
    cpu.instruction_operand_data_ = cpu.readMemory(cpu.data_memory_unit_address_data_);
}

void MOS6502::ZeroPageYAddressingMode(MOS6502& cpu) {
    uint8_t target_address = cpu.readMemory(cpu.program_counter_) + cpu.y_reg_;
    cpu.program_counter_++;

    cpu.data_memory_unit_address_data_ = target_address;
    cpu.instruction_operand_data_ = cpu.readMemory(cpu.data_memory_unit_address_data_);
}

void MOS6502::RelativeAddressingMode(MOS6502& cpu) {
    uint8_t relativeSkip = cpu.readMemory(cpu.program_counter_);
    cpu.program_counter_++;
    cpu.relative_addressing_data_ = *reinterpret_cast<int8_t*>(&relativeSkip);
}

void MOS6502::AbsoluteAddressingMode(MOS6502& cpu) {
    uint8_t address_low_byte = cpu.readMemory(cpu.program_counter_);
    cpu.program_counter_++;
    uint8_t address_high_byte = cpu.readMemory(cpu.program_counter_);
    cpu.program_counter_++;

    cpu.data_memory_unit_address_data_ = (static_cast<uint16_t>(address_high_byte) << 0x08) + static_cast<uint16_t>(address_low_byte);
}
