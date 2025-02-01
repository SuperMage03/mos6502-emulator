// Standard Library Headers
#include <iostream>
#include <chrono>
#include <thread>
// Project Headers
#include "bus.hpp"
#include "mos6502.hpp"
#include "ram.hpp"

int main(int argc, char *argv[]) {
    MOS6502 cpu;
    RAM wram(2048);
    RAM vram(2048);
    BUS bus(cpu, wram, vram);

    while (true) {
        cpu.runCycle();
        cpu.outputCurrentState(std::cout);
        std::this_thread::sleep_for(std::chrono::nanoseconds(std::lrint(MOS6502_CLOCK_PERIOD)));
    }
}
