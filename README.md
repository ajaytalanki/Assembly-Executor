# FISC CPU 

## Overview
This project simulates a finite instruction set computer that allows users to write assembly instructions with a limited set of instructions and registers. The CPU allows 4 operations: ADD, AND, NOT, and BNZ. There are also four 8-bit general purpose registers R0, R1, R2, and R3. Comments can be written using the ';' character.

## Arithmetic Instruction Usage
The usage of each instruction are as follows:  
ADD R0 R1 R2 => (R0 = R1 + R2)  
AND R0 R1 R2 (R0 = R1 & R2)  
NOT R0 R1 (R0 = ~R1)  
BNZ label  

## BNZ Usage
The BNZ instruction is followed by a target address. The program will branch to the specified target address as long as the result of the last operation was not 0. Take a look at the following sample assembly code:  

loop:  
  add r3 r1 r0  ;  r3 = r1 + r0  
  and r0 r1 r1  ;  r0 = r1  
  and r1 r3 r3  ;  r1 = r3  
  bnz loop ; branch to the loop label if the above instruction doesn't result in 0

This program contains an assembler that takes in an assembly file and outputs a machine hex code output file. The simulator then takes the machine hex code file as an input and displays the fetch, execude, and decode cycles of the CPU.   The assembly file allows labels for jumps. The assembler starts by reading the command line arguments. If the user specifies a "-l" flag, the assembler will print out each instruction with its associated line number and machine code at the end of the file. 
