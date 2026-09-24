#include <iostream>
#include <cstdint>
#include <iomanip>
#include <bitset>
#include <array>

class CPU
{
private:
    std::uint8_t ram[4096]{}; //4 KB of ram
    std::uint8_t gen_reg[16]{}; //16 8bit general purpose registers
    std::uint16_t I_reg{}; // 16bit I register
    std::uint16_t PC{0x200}; // Program counter Start at memory location 512 as per instructions
    std::uint8_t SP{}; // Stack pointer
    std::uint16_t stack[16]{}; // Stack 16 16bit values
public:
    void print_state();
    void load_test_data_ram(std::uint16_t idx, std::uint8_t val);
    std::uint16_t fetch();
    std::array<std::uint8_t, 4> decode(std::uint16_t op_code);
    void execute(std::array<std::uint8_t, 4> arr);
    std::uint16_t get_PC();
};

std::uint16_t CPU::fetch(){
    std::uint16_t opcode{};
    std::uint16_t shift_left_8{ram[PC] << 8};
    opcode = shift_left_8 | (ram[PC+1]);
    PC += 2;
    std::cout << "OPCODE: 0x" << std::hex << opcode << '\n';
    return opcode;
}

std::array<std::uint8_t, 4> CPU::decode(std::uint16_t op_code){
    std::uint8_t nib1{(op_code >> 12)};
    std::uint8_t nib2{(op_code >> 8) & 0x000F};
    std::uint8_t nib3{(op_code >> 4) & 0x000F};
    std::uint8_t nib4{(op_code & 0x000F)};
    return {nib1, nib2, nib3, nib4};
}

void CPU::execute(std::array<std::uint8_t, 4> arr){
    std::uint8_t nib1{arr[0]};
    std::uint8_t nib2{arr[1]};
    std::uint8_t nib3{arr[2]};
    std::uint8_t nib4{arr[3]};
    
    if(static_cast<int>(nib1)==6){
        gen_reg[static_cast<int>(nib2)] = (nib3 << 4) | nib4;
    }
    else if(static_cast<int>(nib1)==7){
        gen_reg[static_cast<int>(nib2)] += (nib3 << 4) | nib4;
    }
    else if(nib1==0xA){
        I_reg = (nib2 << 8) | (nib3 << 4) | nib4;
    }
    else if (nib1==0x1)
    {
        PC = (nib2 << 8) | (nib3 << 4) | nib4;
    }
    else if (nib1==0x3)
    {
        if (gen_reg[nib2]==((nib3 << 4) | nib4))
        {
            PC += 2;
        }
    }
    else if (nib1==0x4)
    {
        if (gen_reg[nib2]!=((nib3 << 4) | nib4))
        {
            PC += 2;
        }        
    }
    else if (nib1==0x5 && nib4 == 0x0)
    {
        if (gen_reg[nib2]==gen_reg[nib3])
        {
            PC += 2;
        }   
    }
    else if (nib1==0x9 && nib4==0x0)
    {
        if (gen_reg[nib2]!=gen_reg[nib3])
        {
            PC += 2;
        }
    }
    else if (nib1==0x8)
    {
        if (nib4==0x0)
        {
            gen_reg[nib2]=gen_reg[nib3];
        }
        else if (nib4==0x1)
        {
            gen_reg[nib2]=(gen_reg[nib2] | gen_reg[nib3]);
        }
        else if (nib4==0x2)
        {
            gen_reg[nib2]=(gen_reg[nib2] & gen_reg[nib3]);
        }
        else if (nib4==0x3)
        {
            gen_reg[nib2]=(gen_reg[nib2] ^ gen_reg[nib3]);
        }
        else if (nib4==0x4)
        {
            std::uint16_t temp {static_cast<std::uint16_t>(gen_reg[nib2]) 
                              + static_cast<std::uint16_t>(gen_reg[nib3])};
            if (temp > 0xFF){
                gen_reg[0xF] = 0b1;
            }
            gen_reg[nib2] = static_cast<std::uint8_t>(temp & 0xFF);
        }
        else if (nib4==0x5)
        {
            /* code */
        }
        else if (nib4==0x6)
        {
            /* code */
        }
        else if (nib4==0x7)
        {
            /* code */
        }
        else if (nib4==0x8)
        {
            /* code */
        }
        else if (nib4==0x9)
        {
            /* code */
        }
        else if (nib4==0xA)
        {
            /* code */
        }
        else if (nib4==0xB)
        {
            /* code */
        }
        else if (nib4==0xC)
        {
            /* code */
        }
        else if (nib4==0xD)
        {
            /* code */
        }
        else if (nib4==0xE)
        {
            /* code */
        }
    }
    
    
    
}

std::uint16_t CPU::get_PC()
{
    return PC;
}

void CPU::load_test_data_ram(std::uint16_t idx, std::uint8_t val){
    ram[idx] = val;
}

void CPU::print_state(){
    std::cout << "PC: 0x" << std::hex << PC << '\n';
    std::cout << "SP: " << static_cast<int>(SP) << '\n';
    std::cout << "I_reg: " << std::dec << static_cast<int>(I_reg) << '\n';
    std::cout << "General Registers: " << '\n';
    for(long long unsigned int i = 0; i < std::size(gen_reg); i++){
        std::cout << static_cast<int>(gen_reg[i]) << " * ";
    }
    std::cout << '\n';
}

int main(){
    CPU my_cpu;
    std::uint16_t opcodee{};
    std::array<std::uint8_t, 4> my_arr{};

    my_cpu.load_test_data_ram(0x200, 0x61);
    my_cpu.load_test_data_ram(0x201, 0xFF);
    my_cpu.load_test_data_ram(0x202, 0x63);
    my_cpu.load_test_data_ram(0x203, 0x01);
    my_cpu.load_test_data_ram(0x204, 0x81);
    my_cpu.load_test_data_ram(0x205, 0x34);

    opcodee = my_cpu.fetch();
    my_arr = my_cpu.decode(opcodee);
    my_cpu.execute(my_arr);
    my_cpu.print_state();
    opcodee = my_cpu.fetch();
    my_arr = my_cpu.decode(opcodee);
    my_cpu.execute(my_arr);
    my_cpu.print_state();
    opcodee = my_cpu.fetch();
    my_arr = my_cpu.decode(opcodee);
    my_cpu.execute(my_arr);
    my_cpu.print_state();
    

    // opcodee = my_cpu.fetch();
    // while (opcodee != 0x0)
    // {
    //     my_arr = my_cpu.decode(opcodee);
    //     my_cpu.execute(my_arr);
    //     my_cpu.print_state();
    //     opcodee = my_cpu.fetch();
    // }
    
    std::cout << '\n' << "while loop broken!";
    return 0;
}
