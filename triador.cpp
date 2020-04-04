#undef NDEBUG
#include <cassert>
#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>

using namespace std;

vector<int> memory = {0,0,0,0,0,0,0,0,0,0,0,0,-13};
int borrow_carry = 0;
int pc = 0; // program counter

// The program file must contain a single instruction per line.
// The instruction must be in the first 6 characters of each line, any character beyond first 6 is discarded.
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

void load_program(const char *filename, vector<string> &instructions, vector<int> &arguments) {
    ifstream in;
    in.open (filename, ifstream::in);
    if (in.fail()) return;
    string line;
    while (!in.eof()) {
        getline(in, line);
        if (!line.length()) break;
        string sub = line.substr(0, 6);
        assert(sub.length()==6); // the line is at least 6 characters long

        assert(sub[2]==' ');     // the opcode is separated by a space from the argument

        vector<string> allowed_opcodes = {"EX", "JP", "SK", "OP", "RR", "R1", "R2", "R3", "R4"};
        string opcode = sub.substr(0,2);
        bool ok = false;
        for (string &okcode : allowed_opcodes)
            if (opcode==okcode) ok = true;
        assert(ok);              // is a valid opcode?
        for (size_t i=3; i<6; i++)
            assert(sub[i]=='N' || sub[i]=='O' || sub[i]=='P'); // is a valid 3-trit argument?

        instructions.push_back(opcode);
        int arg = 0;
        for (size_t i=0; i<3; i++) {
            int trit = sub[5-i]=='N' ? -1 : (sub[5-i]=='O' ? 0 : 1);
            int pwr = i==0 ? 1 : (i==1 ? 3 : 9);
            arg += pwr*trit;
        }
        arguments.push_back(arg);
    }
}

void display_memory() {
    for (int i=0; i<13; i++) {
        cout << "R" << (i+1) << ": " << memory[i] << "\t";
    }
    cout << "C: " << borrow_carry  << "\t PC: " << (pc-364) << endl;
}

void execute(vector<string> &opcodes, vector<int> &args) {
    display_memory();
    assert(opcodes.size()==args.size());
    while (pc<(int)opcodes.size()) {
        string oc = opcodes[pc];
        int arg = args[pc];
        if (string("EX") == oc) break;
        if (string("R1") == oc) memory[0] = arg;
        if (string("R2") == oc) memory[1] = arg;
        if (string("R3") == oc) memory[2] = arg;
        if (string("R4") == oc) memory[3] = arg;
        if (string("OP") == oc) cerr << "NEEDS TO BE IMPLEMENTED" << endl;
        if (string("RR") == oc && arg) {
            if (abs(arg)==1) {
                memory[0] += arg;
                if (abs(memory[0])<=13) borrow_carry = 0;
                else if (memory[0]> 13) { borrow_carry =  1; memory[0] -= 13; }
                else if (memory[0]<-13) { borrow_carry = -1; memory[0] += 13; }
            } else {
                memory[arg<0 ? -arg-1 : 0] = memory[arg<0 ? 0 : arg-1];
            }
        }
        if (string("JP") == oc) {
            pc = (memory[12]*27 + arg)+364;
        } else if (string("SK") == oc) {
            if (arg==-1 && borrow_carry==-1) pc++;
            if (arg== 0 && borrow_carry== 0) pc++;
            if (arg== 1 && borrow_carry== 1) pc++;
            if (abs(arg)>1) {
                int R = memory[(abs(arg)-2)/3]; // register
                int C = (abs(arg)-2)%3; // comparison
                if (arg>0 && C==0 && R<0) pc++;
                if (arg>0 && C==1 && R==0) pc++;
                if (arg>0 && C==2 && R>0) pc++;
                if (arg<0 && C==0 && R<=0) pc++;
                if (arg<0 && C==1 && R!=0) pc++;
                if (arg<0 && C==2 && R>=0) pc++;
            }
          pc++;
        } else {
            pc++;
        }
        display_memory();
    }
}

int main(int argc, char** argv) {
    if (argc!=2) {
        cerr << "Usage: " << argv[0] << " program.txt" << endl;
        return 1;
    }

    vector<string> instructions;
    vector<int>    arguments;
    load_program(argv[1], instructions, arguments);

    execute(instructions, arguments);

    return 0;
}

