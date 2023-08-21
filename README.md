# FISC CPU 

## Overview
This project simulates a finite instruction set computer that allows users to write assembly instructions with a limited set of instructions and registers. The CPU works in two stages. First the assembler takes in the .s assembly file as input, and generates a machine hex code .h file. Then the simulator takes in this .h file and prints out the fetch, execute, decode cycles of the CPU. Comments can be written using the ';' character.

## Arithmetic Instruction Usage
There are 3 arithmetic operations ADD, AND, NOT with the following usage:  
ADD R0 R1 R2 => (R0 = R1 + R2)  
AND R0 R1 R2 (R0 = R1 & R2)  
NOT R0 R1 (R0 = ~R1)  

## BNZ Usage  
There is one branch operation BNZ. The BNZ instruction is followed by a target address. The program will branch to the specified target address as long as the result of the last operation was not 0. Take a look at the following sample assembly code:  

loop:  
  add r3 r1 r0  ;  r3 = r1 + r0  
  and r0 r1 r1  ;  r0 = r1  
  and r1 r3 r3  ;  r1 = r3  
  bnz loop ; branch to the loop label if the above instruction doesn't result in 0

## Registers
This CPU allows for four 8-bit registers R0, R1, R2, R3  

## Assembler


The assembler starts by reading the command line arguments. If the user specifies a "-l" flag, the assembler will print out each instruction with its associated line number and machine code at the end of the file. 
