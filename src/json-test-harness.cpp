#include "json-test-harness.hpp"
// Standard Library Includes
#include <iostream>
#include <fstream>

JSONTestHarness::JSONTestHarness(MOS6502& cpu, const std::string& file_path): instructions_tested_{0}, cpu_{cpu} {
    std::ifstream json_file(file_path);
    test_json_ = json::parse(json_file);
}

JSONTestHarness::Result JSONTestHarness::singleInstructionStep() {
    if (instructions_tested_ >= test_json_.size()) {
        std::cout << "All Test Passed" << std::endl;
        return Result::ALL_TESTS_PASSED;
    }
    
    const auto& cur_instruction_test = test_json_[instructions_tested_];
    const auto& cur_instruction_test_initial = cur_instruction_test["initial"];
    const auto& cur_instruction_test_final = cur_instruction_test["final"];
    
    cpu_.program_counter_ = cur_instruction_test_initial["pc"];
    cpu_.stack_ptr_ = cur_instruction_test_initial["s"];
    cpu_.accumulator_ = cur_instruction_test_initial["a"];
    cpu_.x_reg_ = cur_instruction_test_initial["x"];
    cpu_.y_reg_ = cur_instruction_test_initial["y"];
    cpu_.processor_status_.RAW_VALUE = cur_instruction_test_initial["p"];

    for (const auto& address_value_pair : cur_instruction_test_initial["ram"]) {
        cpu_.writeMemory(address_value_pair[0], address_value_pair[1]);
    }

    cpu_.runInstruction();

    std::cout << "Executed Instruction " << cur_instruction_test["name"] << std::endl;

    if (cpu_.program_counter_ != cur_instruction_test_final["pc"]) {
        std::cout << "Unexpected Program Counter: " << cpu_.program_counter_ << std::endl;
        return Result::TEST_FAILED;
    }
    if (cpu_.stack_ptr_ != cur_instruction_test_final["s"]) {
        std::cout << "Unexpected Stack Pointer" << std::endl;
        return Result::TEST_FAILED;
    }
    if (cpu_.accumulator_ != cur_instruction_test_final["a"]) {
        std::cout << "Unexpected Accumulator" << std::endl;
        return Result::TEST_FAILED;
    }
    if (cpu_.x_reg_ != cur_instruction_test_final["x"]) {
        std::cout << "Unexpected X Register" << std::endl;
        return Result::TEST_FAILED;
    }
    if (cpu_.y_reg_ != cur_instruction_test_final["y"]) {
        std::cout << "Unexpected Y Register" << std::endl;
        return Result::TEST_FAILED;
    }
    if (cpu_.processor_status_.RAW_VALUE != cur_instruction_test_final["p"]) {
        std::cout << "Unexpected Processor Status" << std::endl;
        return Result::TEST_FAILED;
    }
    
    for (const auto& address_value_pair : cur_instruction_test_final["ram"]) {
        if (cpu_.readMemory(address_value_pair[0]) != address_value_pair[1]) {
            std::cout << "Unexpected Memory Value at " << address_value_pair[0] << std::endl;
            return Result::TEST_FAILED;
        }
    }

    instructions_tested_++;

    return Result::TEST_OK;
}
