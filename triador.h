#ifndef __TRIADOR_H__
#define __TRIADOR_H__

#include <array>
#include <vector>
#include <utility>

struct Triador {
    Triador();
    void display_memory_state();             // print the memory state to the console
    void assert_memory_state();              // check all the admissible ranges
    void load_program(const char *filename); // fill program array
    void cycle();                            // execute one instruction
    void run(bool verbose = true);           // call cycle() in a loop
    virtual bool do_ex(int){return false;}    // optional EX executor

    std::array<int, 13> R; // 13 registers, valid range for each one -13..+13
    int  C;                // borrow-carry flag, valid values -1,0,+1
    int PC;                // program counter, valid range -364..+364
    std::vector<std::pair<int, int> > program; // read-only program memory, vector of (opcode,argument) pairs
    bool fHalt;            // half flag
};

#endif //__TRIADOR_H__

