#undef NDEBUG
#include <cassert>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

// TRIADOR state:
vector<int> R = {0,0,0,0,0,0,0,0,0,0,0,0,0}; // 13 registers, valid range for each one -13..+13
int  C = 0;                                  // borrow-carry flag, valid values -1,0,+1
int PC = 0;                                  // program counter, valid range -364..+364

void display_memory_state() {
    for (int i=0; i<13; i++)
        assert(abs(R[i])<=13);
    assert(abs(C)<=1);
    assert(abs(PC)<364);

    for (int i=0; i<13; i++) {
        if (i<9) cout << " ";
        cout << "R" << (i+1) << " ";
    }
    cout << " C   PC" << endl;
    for (int i=0; i<13; i++)
        cout << setw(3) << R[i] << " ";
    cout << setw(2) << C  << "  " << setw(4) << (PC-364) << endl << endl;
}

void load_program(const char *filename, vector<string> &opcodes, vector<int> &opargs) {
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

        vector<string> allowed_opcodes = {"EX", "JP", "SK", "OP", "RR", "R1", "R2", "R3", "R4"};
        string opcode = sub.substr(0,2);
        bool ok = false;
        for (string &okcode : allowed_opcodes)
            if (opcode==okcode) ok = true;
        assert(ok);              // is it a valid opcode?

        for (size_t i=3; i<6; i++)
            assert(sub[i]=='N' || sub[i]=='O' || sub[i]=='P'); // do we have a valid 3-trit argument?

        int arg = 0; // convert the 3-trit string (e.g. "NPP") to an actual int value (-5)
        for (size_t i=0; i<3; i++) {
            int trit = sub[5-i]=='N' ? -1 : (sub[5-i]=='O' ? 0 : 1);
            int pwr = i==0 ? 1 : (i==1 ? 3 : 9);
            arg += pwr*trit;
        }

        opcodes.push_back(opcode);
        opargs.push_back(arg);
    }
}

// compute ttt[3] such that value = ttt[0] + 3*ttt[1] + 9*ttt[2]
void decompose(const int value, int ttt[3]) {
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

void execute(const vector<string> &opcodes, const vector<int> &opargs) {
    display_memory_state();
    assert(opcodes.size()==opargs.size());
    while ((size_t)PC<opcodes.size()) {
        string oc = opcodes[PC];
        int arg = opargs[PC];
        if (string("EX") == oc) break;      // halt and catch fire
        if (string("R1") == oc) R[0] = arg; // direct write the the main registers
        if (string("R2") == oc) R[1] = arg;
        if (string("R3") == oc) R[2] = arg;
        if (string("R4") == oc) R[3] = arg;
        if (string("OP") == oc) { // unary tritwise operation
            int ttt_mem[3] = {0,0,0};
            int ttt_arg[3] = {0,0,0};
            int ttt_res[3] = {0,0,0};
            decompose(R[0], ttt_mem);
            decompose(arg,  ttt_arg);
            for (int i=0; i<3; i++)
                ttt_res[i] = ttt_arg[ttt_mem[i]+1];
            R[0] = ttt_res[0] + 3*ttt_res[1] + 9*ttt_res[2];
        }
        if (string("RR") == oc && arg) { // "RR OOO" means "do nothing"
            if (abs(arg)==1) { // RR with -1 or +1 argument is an increment/decrement
                R[0] += arg;
                if (abs(R[0])<=13) C = 0;
                else if (R[0]> 13) { C =  1; R[0] -= 27; } // emulate the overflow
                else if (R[0]<-13) { C = -1; R[0] += 27; } // and set the borrow/carry flag
            } else
                R[arg<0 ? -arg-1 : 0] = R[arg<0 ? 0 : arg-1]; // RR with argument >1 or <-1 is a copy
        }
        if (string("SK") == oc) { // conditional skips of the next operation
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
        }
        PC++; // advance the program counter
        if (string("JP") == oc) { // jump instruction
            PC = (R[12]*27 + arg)+364;
            assert((size_t)PC<opcodes.size());
        }
        display_memory_state();
    }
}

int main(int argc, char** argv) {
    if (argc!=2) {
        cerr << "Usage: " << argv[0] << " ../prog/add.txt" << endl;
        return 1;
    }

    // N.B. the memory is not guaranteed to be initialized!
    std::srand(std::time(nullptr));
    for (size_t i=0; i<13; i++)
        R[i] = std::rand()%27 - 13;

    vector<string> opcodes;
    vector<int>    opargs;
    load_program(argv[1], opcodes, opargs);
    execute(opcodes, opargs);

    return 0;
}

