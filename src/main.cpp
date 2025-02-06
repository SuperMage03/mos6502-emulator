// Standard Library Headers
#include <fstream>
#include <iostream>
#include <chrono>
#include <thread>
// Project Headers
#include "bus.hpp"
#include "mos6502.hpp"
#include "ram.hpp"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "INVALID ARGUEMENTS" << std::endl;
        return 1;
    }

    std::ifstream program_file;
    program_file.open(argv[1], std::ios::binary);
    if (!program_file) {
        std::cerr << "INVALID PROGRAM FILE PROVIDED" << std::endl;
        return 1;
    }

    MOS6502 cpu;
    RAM ram(program_file);
    BUS bus(cpu, ram);

    for (unsigned int i = 0; i < 5; i++) {
    // while (true) {
        cpu.runCycle();
        cpu.outputCurrentState(std::cout);
        std::cout << std::endl;
        std::this_thread::sleep_for(std::chrono::nanoseconds(std::lrint(MOS6502_CLOCK_PERIOD)));
    }

    return 0;
}
