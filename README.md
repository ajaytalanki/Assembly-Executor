# FISC CPU 

## Overview
This project simulates a finite instruction set computer that allows users to write assembly instructions with a limited set of instructions and registers. The CPU works in two stages. First, the assembler takes in the assembly file (.s) as input, and generates a machine hex code file (.hex). Then the simulator takes in this hex file and prints out the fetch, execute, decode cycles of the CPU with the status of each register. Comments can be written using the ';' character.

## Registers
This CPU allows for four 8-bit registers: R0, R1, R2, R3  

## Arithmetic Instruction Usage
There are 3 arithmetic operations ADD, AND, NOT with the following usage:  
ADD R0 R1 R2 => (R0 = R1 + R2)  
AND R0 R1 R2 => (R0 = R1 & R2)  
NOT R0 R1 => (R0 = ~R1)  

## BNZ Usage  
There is one branch operation BNZ. The BNZ instruction is followed by a target address. The program will branch to the specified target address as long as the result of the last operation was not 0. Below is sample assembly code for futher understanding.  

loop:  
  add r3 r1 r0  ;  r3 = r1 + r0  
  and r0 r1 r1  ;  r0 = r1  
  and r1 r3 r3  ;  r1 = r3  
  bnz loop ; branch to the loop label if the above instruction doesn't result in 0

## Assembler
The assembler works using a 2-pass system. In the first pass, the assembler initializes and populates a symbol table consisting of labels along with their associated addresses. In the second pass, the assembler converts each instruction into a machine code representation. This is done by assigning each instruction an OP code along with assigning each register a decimal value. By parsing each line of assembly and using bit shifting operations, each instruction is encoded in hex and output to a .h file.

### Assembler Command-Line Arguments
The assembler requires an assembly file (.s) as input. It optionally takes in a machine code output file to write to, and writes to "fiscas.hex" if no output file is specified. Lastly, there is an optional "-l" flag which prints out all the labels in the program with their associated addresses. This flag will also print out each assembly instruction with its associated machine code representation.  

## Simulator
The simulator takes in the hex file as input. The program parses each line of machine code and uses bit shifting to decide the instruction as well as source and destination registers. The simulator then prints out the status of each register after each instruction as well as the program counter.

### Simulator Command-Line Arguments
The simulator takes in a hex file as the first command-line argument. The simulator also takes an optional "-d" flag which will print the dissasembly of each line of machine code. Lastly, the simulator takes an optional integer argument that defines the number of CPU cycles to print. By default, the simulator will print 20 CPU cycles. 

## Demo
A working demo of this project can be found on my [portfolio](https://ajaytalanki.github.io/portfolio/).
