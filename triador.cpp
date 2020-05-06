#undef NDEBUG
#include <cassert>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <array>
#include <vector>
#include <utility>
#include <cstdlib>
#include <ctime>

#include "triador.h"

using namespace std;

const array<string, 9> allowed_opcodes = {"EX", "JP", "SK", "OP", "RR", "R1", "R2", "R3", "R4"};

Triador::Triador() {
    PC = -364; // the program counter is initalized
    // N.B. the rest of the memory is not guaranteed to be initialized!
    std::srand(std::time(nullptr));
    for (int &r : R)
        r = std::rand()%27 - 13;
    C = std::rand()%2 - 1;

    fHalt = false;
}

void Triador::assert_memory_state() {
    for (const int &r : R)
        assert(abs(r)<=13);
    assert(abs(C)<=1);
    assert(abs(PC)<=364);
}

void Triador::display_memory_state() {
    for (int i=0; i<13; i++) {
        if (i<9) cout << " ";
        cout << "R" << (i+1) << " ";
    }
    cout << " C   PC      opcode" << endl;
    for (const int &r : R)
        cout << setw(3) << r << " ";
    cout << setw(2) << C  << "  " << setw(4) << PC << "     " << allowed_opcodes[program[PC+364].first+4] << " " << program[PC+364].second << endl << endl;
}

void Triador::load_program(const char *filename) {
    // The program file must contain a single instruction per line.
    // The instruction must be in the first 6 characters of each line, any character beyond the first 6 is discarded.
    // Therefore, each line must contain one of the following instructions,
    // where ttt means a 3-trit number with values from NNN (-13) to PPP (+13):
    // EX ttt
    // JP ttt
    // SK ttt
    // OP ttt
    // RR ttt
    // R1 ttt
    // R2 ttt
    // R3 ttt
    // R4 ttt
    program = std::vector<std::pair<int, int> >();
    ifstream in;
    in.open(filename, ifstream::in);
    if (in.fail()) return;
    string line;
    while (!in.eof()) {
        getline(in, line);
        if (!line.length()) break;
        string sub = line.substr(0, 6);

        assert(sub.length()==6); // the line must be at least 6 characters long
        assert(sub[2]==' ');     // the opcode is separated by a space from the argument

        string opcode = sub.substr(0,2);
        int instruction = -5; // out of bounds initialization
        for (int i=0; i<9; i++) {
            if (opcode!=allowed_opcodes[i]) continue;
            instruction = i-4;
            break;
        }
        assert(abs(instruction)<=4); // is it a valid opcode?

        for (size_t i=3; i<6; i++)
            assert(sub[i]=='N' || sub[i]=='O' || sub[i]=='P'); // do we have a valid 3-trit argument?

        int arg = 0; // convert the 3-trit string (e.g. "NPP") to an actual int value (-5)
        for (size_t i=0; i<3; i++) {
            int trit = sub[5-i]=='N' ? -1 : (sub[5-i]=='O' ? 0 : 1);
            int pwr = i==0 ? 1 : (i==1 ? 3 : 9);
            arg += pwr*trit;
        }
        program.push_back(make_pair(instruction, arg));
    }
}

// compute ttt[3] such that value = ttt[0] + 3*ttt[1] + 9*ttt[2]
void binary_to_ternary(const int value, int ttt[3]) {
    assert(abs(value)<=13);
    int n = value;
    bool neg = n < 0;
    if (neg) n = -n;

    for (int i=0; i<3; i++) {
        int r = n % 3; // remainder operator over negative values is implementation-defined, thus bool neg
        if (r == 0)
            ttt[i] = 0;
        else if (r == 1)
            ttt[i] = 1;
        else {
            ttt[i] = -1;
            n++;
        }
        n /= 3;
    }
    if (neg) for (int i=0; i<3; i++)
        ttt[i] = -ttt[i];
}

void Triador::cycle() {
    int opcode = program[PC+364].first;
    int arg    = program[PC+364].second;
    assert(abs(opcode)<=4 && abs(arg)<=13);

    switch (opcode) {
        case -4: { // EX: halt and catch fire if not processed
                     if (!do_ex(arg)) fHalt = true;
                     return;
                 } break;
        case -3: { // JP: jump instruction
                     PC = R[12]*27 + arg;
                     return;
                 } break;
        case -2: { // SK: conditional skips of the next operation
                     if (abs(arg)>1) { // skip w.r.t R1-R4 values
                         int reg = R[(abs(arg)-2)/3]; // register value
                         int cmp = (abs(arg)-2)%3;    // comp operation
                         if (arg>0) {
                             if (cmp==0 && reg <0) PC++;
                             if (cmp==1 && reg==0) PC++;
                             if (cmp==2 && reg >0) PC++;
                         } else {
                             if (cmp==0 && reg<=0) PC++;
                             if (cmp==1 && reg!=0) PC++;
                             if (cmp==2 && reg>=0) PC++;
                         }
                     } else { // skip w.r.t C value
                         if (arg==-1 && C==-1) PC++;
                         if (arg== 0 && C== 0) PC++;
                         if (arg== 1 && C== 1) PC++;
                     }
                 } break;
        case -1: { // OP: unary tritwise operation
                     int ttt_mem[3] = {0,0,0};
                     int ttt_arg[3] = {0,0,0};
                     int ttt_res[3] = {0,0,0};
                     binary_to_ternary(R[0], ttt_mem);
                     binary_to_ternary(arg,  ttt_arg);
                     for (int i=0; i<3; i++)
                         ttt_res[i] = ttt_arg[1-ttt_mem[i]];
                     R[0] = ttt_res[0] + 3*ttt_res[1] + 9*ttt_res[2];
                 } break;
        case 0: { // RR: copying between registers
                    if (!arg) break; // "RR OOO" means "do nothing"
                    if (abs(arg)==1) { // RR with -1 or +1 argument is an increment/decrement
                        R[0] += arg;
                        if (abs(R[0])<=13) C = 0;
                        else if (R[0]> 13) { C =  1; R[0] -= 27; } // emulate the overflow
                        else if (R[0]<-13) { C = -1; R[0] += 27; } // and set the borrow/carry flag
                    } else
                        R[arg<0 ? -arg-1 : 0] = R[arg<0 ? 0 : arg-1]; // RR with argument >1 or <-1 is a copy
                } break;
        case 1: { // R1: write arg to the register R1
                    R[0] = arg;
                } break;
        case 2: { // R2: write arg to the register R2
                    R[1] = arg;
                } break;
        case 3: { // R3: write arg to the register R3
                    R[2] = arg;
                } break;
        case 4: { // R4: write arg to the register R4
                    R[3] = arg;
                } break;
    }

    PC++; // advance the program counter
}

void Triador::run(bool verbose) {
    assert_memory_state();
    if (verbose) display_memory_state();
    while (1) {
        cycle();
        assert_memory_state();

        if (PC+364>=(int)program.size()) {
            std::cerr << "Warning: PC points outside the program, halting Triador" << std::endl;
            break;
        }
        if (verbose) display_memory_state();
        if (fHalt) break; // halt and catch fire
    }
}
