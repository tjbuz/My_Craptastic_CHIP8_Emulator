#include <iostream>
#include <cstdint>
#include <iomanip>
#include <bitset>
#include <array>
#include <random>
#include <SDL3\SDL.h>

class Display
{
private:
    int width{640};
    int height{320};
    SDL_Window * window{nullptr};
    SDL_Renderer * renderer{nullptr};

    const bool * key_states = SDL_GetKeyboardState(nullptr);
public:
    Display(/* args */);
    std::uint16_t my_key_val();
};

Display::Display(/* args */){
    window = SDL_CreateWindow("xX_CrApTaStIc CHIP 8_Xx", width, height, 0);
    renderer = SDL_CreateRenderer(window, nullptr);
    bool game_run{true};

    SDL_Event event;

    while (game_run)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                game_run = false;
            }
            
        }
        
    }
    
    SDL_DestroyWindow(window);
    SDL_Quit();
}

std::uint16_t Display::my_key_val(){
    if (key_states[SDL_SCANCODE_1]) {    
        return 0x1;
    }
}


class CPU
{
private:
    std::uint8_t ram[4096]{}; //4 KB of ram
    std::uint8_t gen_reg[16]{}; //16 8bit general purpose registers
    std::uint16_t I_reg{}; // 16bit I register
    std::uint16_t PC{0x200}; // Program counter Start at memory location 512 as per instructions
    std::uint8_t SP{}; // Stack pointer
    std::uint16_t stack[16]{}; // Stack 16 16bit values

    bool screen[64][32]{}; // Dont know SDL2 so implimenting using arrays instead for time being
    //keypad[16]{0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8,0x9,0xA,0xB,0xC,0xD,0xE,0xF};

    int delay_timer{};
    int sound_timer{};

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
            if (gen_reg[nib2] > gen_reg[nib3]){
                gen_reg[0xF] = 0b1;
            }
            else
            {
                gen_reg[0xF] = 0b0;
            }
            gen_reg[nib2] -= gen_reg[nib3];
            
        }
        else if (nib4==0x6)
        {
            if (gen_reg[nib2] & 0x1 == 0x1){
                gen_reg[0xF] = 0x1;
            }
            gen_reg[nib2] >>= 1;
        }
        else if (nib4==0x7)
        {
            if (gen_reg[nib3] > gen_reg[nib2]){
                gen_reg[0xF] = 0b1;
            }
            else
            {
                gen_reg[0xF] = 0b0;
            }
            gen_reg[nib2] = gen_reg[nib3] - gen_reg[nib2];
        }
        else if (nib4==0xE)
        {
            if ((gen_reg[nib2] >> 3) & 0x1 == 0x1){
                gen_reg[0xF] = 0x1;
            }
            gen_reg[nib2] <<= 1;
        }
    }
    else if (nib1==0x2)
    {
        stack[SP] = PC;
        SP += 1;
        PC = (nib2 << 8) | (nib3 << 4) | nib4;
    }
    else if (nib1==0x0 && nib2==0x0 && nib3==0xE && nib4==0xE)
    {
        PC = stack[0];
        SP -= 1;
    }
    else if (nib1==0xB)
    {
        PC = ((nib2 << 8) | (nib3 << 4) | nib4) + gen_reg[0];
    }
    else if (nib1==0xC)
    {
        static std::random_device rd;
        static std::mt19937 engine(rd());
        static std::uniform_int_distribution<int> distrib(0, 255);

        std::uint8_t my_rand = static_cast<std::uint8_t>(distrib(engine));

        std::cout<<"random: "<< static_cast<std::bitset<8>>(my_rand);
        gen_reg[nib2] = ((nib3 << 4) | nib4) & my_rand;
    }
    else if (nib1==0xF)
    {
        if (nib3==0x0 && nib4==0x7)
        {
            gen_reg[nib2] = delay_timer;
        }
        else if (nib3==0x0 && nib4==0xA)
        {
            SDL_Event event;

            bool waiting = true;
            while (waiting) {
                SDL_WaitEvent(&event);

                if (event.type == SDL_EVENT_KEY_UP) {
                    std::cout << "key value: " << Display.my_key_val();
                    waiting = false;   // continue program
                }
            }
        }
        else if (nib3==0x1 && nib4==0x5)
        {
            delay_timer = gen_reg[nib2];
        }
        else if (nib3==0x1 && nib4==0x8)
        {
            sound_timer = gen_reg[nib2];
        }
        else if (nib3==0x1 && nib4==0xE)
        {
            I_reg += gen_reg[nib2];
        }
        else if (nib3==0x2 && nib4==0x9)
        {
            /*continue implementing instructions*/
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
    Display my_disp;
}

/*int main(){
    CPU my_cpu;
    std::uint16_t opcodee{};
    std::array<std::uint8_t, 4> my_arr{};

    my_cpu.load_test_data_ram(0x200, 0x61);
    my_cpu.load_test_data_ram(0x201, 0xFF);
    my_cpu.load_test_data_ram(0x202, 0x63);
    my_cpu.load_test_data_ram(0x203, 0x01);
    my_cpu.load_test_data_ram(0x204, 0xC1);
    my_cpu.load_test_data_ram(0x205, 0x11);

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
}*/
