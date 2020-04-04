# Ternary computer program emulator

Many claimed to build a ternary computer, however nobody (to the best of my knowledge) completed the project. Triador project makes no empty promises!

I am building a very simple but functional 3-trit balanced ternary computer. The only building block allowed is a ternary multiplexer based on DG403 analog switches. Check this youtube mini-series about the hardware:

[![](doc/memory-board.jpg)](https://www.youtube.com/playlist?list=PL9MBW6e0V7UIvP2vY_aKwsu93wqYq5jXJ)

This repository contains a program emulator that eases the debugging of the hardware.

# compile and execute a sample program
```sh
git clone https://github.com/ssloy/triador.git
cd triador
mkdir build
cd build
cmake ..
make
./triador ../prog/add.txt
```

This executes [a very simple program](prog/add.txt) that writes two numbers to the registers R2 and R3 and computes their sum.
The result is stored in R3.
The emulator drops the computer state for all steps.

Here is the execution log:
```
$ ./triador ../prog/add.txt
R1: 0   R2: 0   R3: 0   R4: 0   R5: 0   R6: 0   R7: 0   R8: 0   R9: 0   R10: 0  R11: 0  R12: 0  R13: -13        C: 0     PC: -364
R1: 0   R2: -2  R3: 0   R4: 0   R5: 0   R6: 0   R7: 0   R8: 0   R9: 0   R10: 0  R11: 0  R12: 0  R13: -13        C: 0     PC: -363
R1: 0   R2: -2  R3: 13  R4: 0   R5: 0   R6: 0   R7: 0   R8: 0   R9: 0   R10: 0  R11: 0  R12: 0  R13: -13        C: 0     PC: -362
R1: 13  R2: -2  R3: 13  R4: 0   R5: 0   R6: 0   R7: 0   R8: 0   R9: 0   R10: 0  R11: 0  R12: 0  R13: -13        C: 0     PC: -361
R1: 13  R2: -2  R3: 13  R4: 0   R5: 0   R6: 0   R7: 0   R8: 0   R9: 0   R10: 0  R11: 0  R12: 0  R13: -13        C: 0     PC: -359
R1: 13  R2: -2  R3: 13  R4: 0   R5: 0   R6: 0   R7: 0   R8: 0   R9: 0   R10: 0  R11: 0  R12: 0  R13: -13        C: 0     PC: -358
R1: 12  R2: -2  R3: 13  R4: 0   R5: 0   R6: 0   R7: 0   R8: 0   R9: 0   R10: 0  R11: 0  R12: 0  R13: -13        C: 0     PC: -357
R1: 12  R2: -2  R3: 13  R4: 0   R5: 0   R6: 0   R7: 0   R8: 0   R9: 0   R10: 0  R11: 0  R12: 0  R13: -13        C: 0     PC: -355
R1: 12  R2: -2  R3: 12  R4: 0   R5: 0   R6: 0   R7: 0   R8: 0   R9: 0   R10: 0  R11: 0  R12: 0  R13: -13        C: 0     PC: -354
R1: -2  R2: -2  R3: 12  R4: 0   R5: 0   R6: 0   R7: 0   R8: 0   R9: 0   R10: 0  R11: 0  R12: 0  R13: -13        C: 0     PC: -353
R1: -2  R2: -2  R3: 12  R4: 0   R5: 0   R6: 0   R7: 0   R8: 0   R9: 0   R10: 0  R11: 0  R12: 0  R13: -13        C: 0     PC: -352
R1: -1  R2: -2  R3: 12  R4: 0   R5: 0   R6: 0   R7: 0   R8: 0   R9: 0   R10: 0  R11: 0  R12: 0  R13: -13        C: 0     PC: -351
R1: -1  R2: -2  R3: 12  R4: 0   R5: 0   R6: 0   R7: 0   R8: 0   R9: 0   R10: 0  R11: 0  R12: 0  R13: -13        C: 0     PC: -349
R1: -1  R2: -1  R3: 12  R4: 0   R5: 0   R6: 0   R7: 0   R8: 0   R9: 0   R10: 0  R11: 0  R12: 0  R13: -13        C: 0     PC: -348
R1: -1  R2: -1  R3: 12  R4: 0   R5: 0   R6: 0   R7: 0   R8: 0   R9: 0   R10: 0  R11: 0  R12: 0  R13: -13        C: 0     PC: -362
R1: 12  R2: -1  R3: 12  R4: 0   R5: 0   R6: 0   R7: 0   R8: 0   R9: 0   R10: 0  R11: 0  R12: 0  R13: -13        C: 0     PC: -361
R1: 12  R2: -1  R3: 12  R4: 0   R5: 0   R6: 0   R7: 0   R8: 0   R9: 0   R10: 0  R11: 0  R12: 0  R13: -13        C: 0     PC: -359
R1: 12  R2: -1  R3: 12  R4: 0   R5: 0   R6: 0   R7: 0   R8: 0   R9: 0   R10: 0  R11: 0  R12: 0  R13: -13        C: 0     PC: -358
R1: 11  R2: -1  R3: 12  R4: 0   R5: 0   R6: 0   R7: 0   R8: 0   R9: 0   R10: 0  R11: 0  R12: 0  R13: -13        C: 0     PC: -357
R1: 11  R2: -1  R3: 12  R4: 0   R5: 0   R6: 0   R7: 0   R8: 0   R9: 0   R10: 0  R11: 0  R12: 0  R13: -13        C: 0     PC: -355
R1: 11  R2: -1  R3: 11  R4: 0   R5: 0   R6: 0   R7: 0   R8: 0   R9: 0   R10: 0  R11: 0  R12: 0  R13: -13        C: 0     PC: -354
R1: -1  R2: -1  R3: 11  R4: 0   R5: 0   R6: 0   R7: 0   R8: 0   R9: 0   R10: 0  R11: 0  R12: 0  R13: -13        C: 0     PC: -353
R1: -1  R2: -1  R3: 11  R4: 0   R5: 0   R6: 0   R7: 0   R8: 0   R9: 0   R10: 0  R11: 0  R12: 0  R13: -13        C: 0     PC: -352
R1: 0   R2: -1  R3: 11  R4: 0   R5: 0   R6: 0   R7: 0   R8: 0   R9: 0   R10: 0  R11: 0  R12: 0  R13: -13        C: 0     PC: -351
R1: 0   R2: -1  R3: 11  R4: 0   R5: 0   R6: 0   R7: 0   R8: 0   R9: 0   R10: 0  R11: 0  R12: 0  R13: -13        C: 0     PC: -349
R1: 0   R2: 0   R3: 11  R4: 0   R5: 0   R6: 0   R7: 0   R8: 0   R9: 0   R10: 0  R11: 0  R12: 0  R13: -13        C: 0     PC: -348
R1: 0   R2: 0   R3: 11  R4: 0   R5: 0   R6: 0   R7: 0   R8: 0   R9: 0   R10: 0  R11: 0  R12: 0  R13: -13        C: 0     PC: -362
R1: 11  R2: 0   R3: 11  R4: 0   R5: 0   R6: 0   R7: 0   R8: 0   R9: 0   R10: 0  R11: 0  R12: 0  R13: -13        C: 0     PC: -361
R1: 11  R2: 0   R3: 11  R4: 0   R5: 0   R6: 0   R7: 0   R8: 0   R9: 0   R10: 0  R11: 0  R12: 0  R13: -13        C: 0     PC: -360
R1: 11  R2: 0   R3: 11  R4: 0   R5: 0   R6: 0   R7: 0   R8: 0   R9: 0   R10: 0  R11: 0  R12: 0  R13: -13        C: 0     PC: -347
```

Triador has the following instruction set:

![](doc/triador_instruction_set.png)
