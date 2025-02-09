// Standard Library Headers
#include <iostream>
#include <sstream>
// Project Headers
#include "bus.hpp"
#include "mos6502.hpp"
#include "ram.hpp"
#include "json-test-harness.hpp"

static std::string uint8_to_hex_string(const uint8_t& value) {
    std::stringstream ss;
    ss << std::hex << std::setw(2) << std::setfill('0') << unsigned(value);
    return ss.str();
}

int main(int argc, char *argv[]) {
    MOS6502 cpu;
    RAM ram(65536); // 64kB for testing
    BUS bus(cpu, ram);
    
    for (unsigned int i = 0; i < MOS6502::instruction_lookup_table.size(); i++) {
        const MOS6502::Instruction& instruction = MOS6502::instruction_lookup_table.at(i);
        // Skip Unofficial OPCODE Tests
        if (instruction.name == "???") continue;

        // Download JSON tests from https://github.com/SingleStepTests/ProcessorTests/tree/main/nes6502/v1
        //   Create a folder named "json-tests" and place all NES 6502 tests in there and run the program
        //   Program should exit with code 0 if succeed and 1 if failed
        JSONTestHarness json_test_harness{cpu, "json-tests/" + uint8_to_hex_string(i) + ".json"};

        while (true) {
            JSONTestHarness::Result step_result = json_test_harness.singleInstructionStep();
            if (step_result == JSONTestHarness::Result::ALL_TESTS_PASSED) {
                break;
            }
            if (step_result == JSONTestHarness::Result::TEST_FAILED) {
                return 1;
            }
        }
    }
    return 0;
}
