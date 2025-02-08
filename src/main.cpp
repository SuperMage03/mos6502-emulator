// Standard Library Headers
#include <fstream>
#include <iostream>
#include <chrono>
#include <thread>
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
        if (instruction.name == "???") continue;

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

    // if (argc < 2) {
    //     std::cerr << "INVALID ARGUEMENTS" << std::endl;
    //     return 1;
    // }

    // std::ifstream program_file;
    // program_file.open(argv[1], std::ios::binary);
    // if (!program_file) {
    //     std::cerr << "INVALID PROGRAM FILE PROVIDED" << std::endl;
    //     return 1;
    // }

    // MOS6502 cpu;
    // RAM ram(program_file);
    // BUS bus(cpu, ram);

    // if (argc > 2) {
    //     cpu.setProgramCounter(std::stoul(argv[2], nullptr, 16));
    // }

    // // for (unsigned int i = 0; i < 30; i++) {
    // while (true) {
    //     cpu.runCycle();
    //     // cpu.outputCurrentState(std::cout);
    //     // std::cout << std::endl;
    //     std::this_thread::sleep_for(std::chrono::nanoseconds(std::lrint(MOS6502_CLOCK_PERIOD)));
    // }

    return 0;
}
