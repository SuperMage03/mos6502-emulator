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
    
    const uint64_t old_cycle = cpu_.getCyclesElapsed();

    const auto& cur_instruction_test = test_json_[instructions_tested_];
    const auto& cur_instruction_test_initial = cur_instruction_test["initial"];
    const auto& cur_instruction_test_final = cur_instruction_test["final"];
    
    // Sets the initial state of the CPU
    const MOS6502::State initial_state = {
        cur_instruction_test_initial["pc"],
        cur_instruction_test_initial["s"],
        cur_instruction_test_initial["a"],
        cur_instruction_test_initial["x"],
        cur_instruction_test_initial["y"],
        cur_instruction_test_initial["p"]
    };
    cpu_.setState(initial_state);

    // Sets the initial state of the Memory
    for (const auto& address_value_pair : cur_instruction_test_initial["ram"]) {
        cpu_.writeMemory(address_value_pair[0], address_value_pair[1]);
    }

    cpu_.runInstruction();
    std::cout << "Executed Instruction " << cur_instruction_test["name"] << std::endl;

    if (cpu_.getCyclesElapsed() - old_cycle != cur_instruction_test["cycles"].size()) {
        std::cout << "Unexpected Cycle Count" << std::endl;
        std::cout << "Got " << cpu_.getCyclesElapsed() - old_cycle << " Expected " << cur_instruction_test["cycles"].size() << std::endl;
        return Result::TEST_FAILED;
    }

    // ----------------------- Checking the CPU State --------------------------

    const MOS6502::State final_state = cpu_.getState();

    if (final_state.program_counter != cur_instruction_test_final["pc"]) {
        std::cout << "Unexpected Program Counter" << std::endl;
        std::cout << "Got " << final_state.program_counter << " Expected " << cur_instruction_test_final["pc"] << std::endl;
        return Result::TEST_FAILED;
    }
    if (final_state.stack_ptr != cur_instruction_test_final["s"]) {
        std::cout << "Unexpected Stack Pointer" << std::endl;
        std::cout << "Got " << final_state.stack_ptr << " Expected " << cur_instruction_test_final["s"] << std::endl;
        return Result::TEST_FAILED;
    }
    if (final_state.accumulator != cur_instruction_test_final["a"]) {
        std::cout << "Unexpected Accumulator" << std::endl;
        std::cout << "Got " << final_state.accumulator << " Expected " << cur_instruction_test_final["a"] << std::endl;
        return Result::TEST_FAILED;
    }
    if (final_state.x_reg != cur_instruction_test_final["x"]) {
        std::cout << "Unexpected X Register" << std::endl;
        std::cout << "Got " << final_state.x_reg << " Expected " << cur_instruction_test_final["x"] << std::endl;
        return Result::TEST_FAILED;
    }
    if (final_state.y_reg != cur_instruction_test_final["y"]) {
        std::cout << "Unexpected Y Register" << std::endl;
        std::cout << "Got " << final_state.y_reg << " Expected " << cur_instruction_test_final["y"] << std::endl;
        return Result::TEST_FAILED;
    }
    if (final_state.processor_status != cur_instruction_test_final["p"]) {
        std::cout << "Unexpected Processor Status" << std::endl;
        std::cout << "Got " << final_state.processor_status << " Expected " << cur_instruction_test_final["p"] << std::endl;
        return Result::TEST_FAILED;
    }
    
    // ----------------------- Checking the Memory -----------------------------

    for (const auto& address_value_pair : cur_instruction_test_final["ram"]) {
        if (cpu_.readMemory(address_value_pair[0]) != address_value_pair[1]) {
            std::cout << "Unexpected memory value at address" << address_value_pair[0] << std::endl;
            std::cout << "Got " << cpu_.readMemory(address_value_pair[0]) << " Expected " << address_value_pair[1] << std::endl;
            return Result::TEST_FAILED;
        }
    }

    instructions_tested_++;

    return Result::TEST_OK;
}
