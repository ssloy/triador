# The ultimate goal: a ternary computer

Many claimed to build a ternary computer, however nobody (to the best of my knowledge) completed the project. Triador project makes no empty promises!

I am building a very simple but functional 3-trit balanced ternary computer. The only building block allowed is a ternary multiplexer based on DG403 analog switches. Check this youtube mini-series about the hardware:

[![](doc/memory-board.jpg)](https://www.youtube.com/playlist?list=PL9MBW6e0V7UIvP2vY_aKwsu93wqYq5jXJ)

# Computer description / instruction set
This repository contains a program emulator that eases the debugging of the hardware. Triador has a 3-trit architecture, this means that its registers can store numbers from -13 to +13. It has four main registers R1-R4 and nine extra registers R5-R13. Note that R13 is a special register used to choose current program memory segment (more on this below). So, in total, Triador can store 13 numbers from the [-13..+13] range. In addition to that, it has a 1-trit borrow/carry flag and a 6-trit program counter register. The read-only program memory has 27 segments, 27 instructions per segment. So, max program size is 729 instructions. Here is a graphic description:

![](doc/triador-description.png)

Triador has the a very limited instruction set. It has 9 instructions; each instruction has a mandatory 3-trit argument. Note that the extension instruction at the moment is interpreted as [halt and catch fire](https://en.wikipedia.org/wiki/Halt_and_Catch_Fire_(computing)). Here is the complete list of available instructions:

![](doc/triador-instructions.png)

# Compilation / execution
```sh
git clone https://github.com/ssloy/triador.git
cd triador
mkdir build
cd build
cmake ..
make
./triador ../prog/add.txt
```
The program file must contain a single instruction per line. The instruction must be in the first 6 characters of each line, any character beyond the first 6 is discarded. Therefore, each line must contain one of the following instructions,
where ttt means a 3-trit number with values from NNN (-13) to PPP (+13):
* EX ttt
* JP ttt
* SK ttt
* OP ttt
* RR ttt
* R1 ttt
* R2 ttt
* R3 ttt
* R4 ttt

The emulator prints the complete state of the Triador computer for all steps. Note that in its current form any call for "EX ttt" means "halt and catch fire".

# example programs
## Add
Here is [a very simple program](prog/add.txt) that writes two numbers to the registers R2 and R3 and computes their sum. The result is stored in R3:

![](https://raw.githubusercontent.com/ssloy/triador/master/doc/add.png)

The first column (6 first characters of each line) is the actual program. The program counter (PC) is initialized to NNN NNN. Recall that any character beyond the first 6 is considered to be a comment.

Here is an execution log:
```
$ ./triador ../prog/add.txt | tail -n 3
 R1  R2  R3  R4  R5  R6  R7  R8  R9 R10 R11 R12 R13  C   PC
 11   0  11   5   0   6 -12  -2  11   8  11 -10 -13  0  -345
```
Note that R3 contains the result of -2 + 13.

## Add with overflow control

![](https://raw.githubusercontent.com/ssloy/triador/master/doc/add-with-overflow-control.png)

## 6-trit addition

![](https://raw.githubusercontent.com/ssloy/triador/master/doc/long-add.png)

## Greatest common divisor

![](https://raw.githubusercontent.com/ssloy/triador/master/doc/gcd.png)
