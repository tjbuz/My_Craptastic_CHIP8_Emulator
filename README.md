# My_Craptastic_CHIP8_Emulator
First major project using C++17 to program a CHIP8 emulator

CHIP-8 emulator using Cowgod's technical reference http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#6xkk

Part I - Build the CPU;
-4096 bytes (4 KB) of memory
-16 general-purpose 8-bit registers, usually called V0 through VF
-the special I register is 16-bit
-the program counter is 16-bit starts at memory location 512 as per ref
-stack pointer only needs to index the 16-entry stack
-stack is 16 16-bit addresses

overview of goal:

CPU state
   ↓
Initialise/reset CPU:
Note 
   ↓
Load ROM into memory: 
use load_test_data_ram() method initially before loading ROM
   ↓
Fetch opcode:
CHIP-8 instructions are 2 bytes long need to retrieve 
RAM[0x200] → 0x60
RAM[0x201] → 0x05
Need to combine so opcode looks like opcode = 0x6005
PC needs to increment by 2 because of this
   ↓
Decode opcode:
our example 0x6005 decode is just figure out what this means
6 = instruction type
0 = register number
05 = 8-bit value 
so can see we are using the 6xkk instruction
   ↓
Execute instruction:
in our example do this:
6xkk - LD Vx, byte
Set Vx = kk.

The interpreter puts the value kk into register Vx.
   ↓
Repeat

At this point implement all instructions into CPU checking each (check overflow) using tests from the load_test_data_ram() recommended approach:
Easy:
6xkk    Vx = kk /            
7xkk    Vx += kk /          
Annn    I = nnn 
1nnn    PC = nnn
3xkk    skip if Vx == kk
4xkk    skip if Vx != kk
5xy0    skip if Vx == Vy
9xy0    skip if Vx != Vy
register ops:
8xy0    Vx = Vy
8xy1    Vx |= Vy
8xy2    Vx &= Vy
8xy3    Vx ^= Vy
8xy4    Vx += Vy
8xy5    Vx -= Vy
8xy6    shift Vx right
8xy7    Vx = Vy - Vx
8xyE    shift Vx left
Then
2nnn    CALL
00EE    RET

