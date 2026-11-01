#include <iostream>
#include <cstdint>
#include <iomanip>
#include <bitset>
#include <array>
#include <random>
#include <SDL3/SDL.h>

class Display
{
private:
    int width{640};
    int height{320};
    SDL_Window * window{nullptr};
    SDL_Renderer * renderer{nullptr};
    SDL_Event event;
public:
    Display(/* args */);
    ~Display();
    int my_key_val();
    bool pollevents();
    void clear();
    void draw(int vx, int vy, std::vector<std::array<int, 8>> vect);
    void render_screen();
    int screen[32][64]{0};
    bool set_vf = false;
};

void Display::render_screen(){
    SDL_SetRenderDrawColor(renderer, 10, 10, 80, 255);
    SDL_RenderClear(renderer);

    for(int i = 0; i < 64; i++){
        for(int j = 0; j < 32; j++){

            if(screen[j][i] == 1){
                SDL_FRect red_apple{10*(i), 10*(j), 10, 10};
                SDL_SetRenderDrawColor(renderer, 255, 225, 0, 255);
                SDL_RenderFillRect(renderer, &red_apple);
            } 
            else if (screen[j][i] == 0){
                SDL_FRect bg{10*(i), 10*(j), 10, 10};
                SDL_SetRenderDrawColor(renderer, 10, 10, 80, 255);
                SDL_RenderFillRect(renderer, &bg);
            }
            else{
                std::cerr << "ERROR: ";
            }
        }
    }
    SDL_RenderPresent(renderer);

}

void Display::clear(){
    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 64; x++) {
            screen[y][x] = 0;
        }
    }
}

void Display::draw(int vx, int vy, std::vector<std::array<int, 8>> vect){

    int temp[32][64]{};
    for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 64; x++) {
            temp[y][x] = screen[y][x];
        }
    }

    for(int i = 0; i < vect.size(); i++){
        for(int j = 0; j < 8; j++){
            int vxx = (vx+j)%64;
            int vyy = (vy+i)%32;

            if (temp[vyy][vxx] == 1 && vect[i][j] == 1){
                set_vf = true;
            }
            screen[vyy][vxx] = temp[vyy][vxx] ^ vect[i][j];
        }
    }
}

Display::Display(/* args */){
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << "SDL_Init failed: "
                  << SDL_GetError() << '\n';
        return;
    }

    window = SDL_CreateWindow("xX_CrApTaStIc CHIP 8_Xx", width, height, 0);
    renderer = SDL_CreateRenderer(window, nullptr);
    bool game_run{true};
    
}

Display::~Display(){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int Display::my_key_val(){
    const bool * key_states = SDL_GetKeyboardState(nullptr);
    if (key_states[SDL_SCANCODE_1]){
        std::cout << '\n' << "key value: " << 1 << '\n';
        return 1;
    }
    if (key_states[SDL_SCANCODE_2]){
        std::cout << '\n' << "key value: " << 2 << '\n';
        return 2;
    }
    if (key_states[SDL_SCANCODE_3]){
        std::cout << '\n' << "key value: " << 3 << '\n';
        return 3;
    }
    if (key_states[SDL_SCANCODE_4]){
        std::cout << '\n' << "key value: " << 12 << '\n';
        return 12;
    }
    if (key_states[SDL_SCANCODE_Q]){
        std::cout << '\n' << "key value: " << 4 << '\n';
        return 4;
    }
    if (key_states[SDL_SCANCODE_W]){
        std::cout << '\n' << "key value: " << 5 << '\n';
        return 5;
    }
    if (key_states[SDL_SCANCODE_E]){
        std::cout << '\n' << "key value: " << 6 << '\n';
        return 6;
    }
    if (key_states[SDL_SCANCODE_R]){
        std::cout << '\n' << "key value: " << 13 << '\n';
        return 13;
    }
    if (key_states[SDL_SCANCODE_A]){
        std::cout << '\n' << "key value: " << 7 << '\n';
        return 7;
    }
    if (key_states[SDL_SCANCODE_S]){
        std::cout << '\n' << "key value: " << 8 << '\n';
        return 8;
    }
    if (key_states[SDL_SCANCODE_D]){
        std::cout << '\n' << "key value: " << 9 << '\n';
        return 9;
    }
    if (key_states[SDL_SCANCODE_F]){
        std::cout << '\n' << "key value: " << 14 << '\n';
        return 14;
    }
    if (key_states[SDL_SCANCODE_Z]){
        std::cout << '\n' << "key value: " << 10 << '\n';
        return 10;
    }
    if (key_states[SDL_SCANCODE_X]){
        std::cout << '\n' << "key value: " << 0 << '\n';
        return 0;
    }
    if (key_states[SDL_SCANCODE_C]){
        std::cout << '\n' << "key value: " << 11 << '\n';
        return 11;
    }
    if (key_states[SDL_SCANCODE_V]){
        std::cout << '\n' << "key value: " << 15 << '\n';
        return 15;
    }
    return -1;
}

bool Display::pollevents(){
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            return false;
        }
    }
    return true;
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

    int delay_timer{};
    int sound_timer{};

    std::uint16_t sprites[16][5] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0,
    0x20, 0x60, 0x20, 0x20, 0x70,
    0xF0, 0x10, 0xF0, 0x80, 0xF0,
    0xF0, 0x10, 0xF0, 0x10, 0xF0,
    0x90, 0x90, 0xF0, 0x10, 0x10,
    0xF0, 0x80, 0xF0, 0x10, 0xF0,
    0xF0, 0x80, 0xF0, 0x90, 0xF0,
    0xF0, 0x10, 0x20, 0x40, 0x40,
    0xF0, 0x90, 0xF0, 0x90, 0xF0,
    0xF0, 0x90, 0xF0, 0x10, 0xF0,
    0xF0, 0x90, 0xF0, 0x90, 0x90,
    0xE0, 0x90, 0xE0, 0x90, 0xE0,
    0xF0, 0x80, 0x80, 0x80, 0xF0,
    0xE0, 0x90, 0x90, 0x90, 0xE0,
    0xF0, 0x80, 0xF0, 0x80, 0xF0,
    0xF0, 0x80, 0xF0, 0x80, 0x80
    };

public:
    CPU();
    bool wait_key = false;
    bool clear_display = false;
    bool call_draw = false;
    std::vector<std::array<int, 8>> bits;
    int draw_reg_x{};
    int draw_reg_y{};
    std::uint8_t wait_reg{};
    void print_state();
    void load_test_data_ram(std::uint16_t idx, std::uint8_t val);
    std::uint16_t fetch();
    std::array<std::uint8_t, 4> decode(std::uint16_t op_code);
    void execute(std::array<std::uint8_t, 4> arr);
    std::uint16_t get_PC();
    void update_gen_reg(int regnum, std::uint8_t reg_val);
};

CPU::CPU(){
    for(int i = 0; i < 16; i++){
        for (int j = 0; j < 5; j++){
            ram[(0x050)+(i*5)+j] = sprites[i][j];
        }  
    }
}

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
            //I want to wait while the user presses a key and then continue executing
            wait_key = true;
            wait_reg = nib2;
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
            I_reg = 0x050 + (gen_reg[nib2]*5);
        }
        else if (nib3==0x3 && nib4==0x3)
        {
            uint16_t temp = gen_reg[nib2];

            ram[I_reg] = temp/100;
            ram[I_reg+1] = (temp/10)%10;
            ram[I_reg+2] = temp%10;

            std::cout << "hundreds: " << temp/100 << '\n';
            std::cout << "Tens: " << (temp/10)%10 << '\n';
            std::cout << "ones: " << temp%10 << '\n'; 
        }
        else if (nib3==0x5 && nib4==0x5)
        {
            for(int i = 0; i <= static_cast<int>(nib2); i++){
                ram[I_reg + i] = gen_reg[i];
            }
        }
        else if (nib3==0x6 && nib4==0x5)
        {
            for(int i = 0; i <= static_cast<int>(nib2); i++){
                gen_reg[i] = ram[I_reg + i];
            }
        }    
    }    
    else if (nib1==0x0 && nib2==0x0 && nib3==0xE && nib4==0x0)
    {
        clear_display = true;
    }
    else if (nib1==0xD)
    {
        bits.resize(static_cast<int>(nib4));
        std::cout << '\n' << "Sprite height: " << static_cast<int>(nib4) << '\n';
        for(int i = 0; i < static_cast<int>(nib4); i++){
            for(int idx = 7; idx >= 0; idx--){
                bits[i][7-idx] = (ram[I_reg+i] >> idx) & 0x01;
                std::cout << '\n' << "bits n bobs:" << static_cast<int>(bits[i][7-idx]) << '\n';
            }
        }

        draw_reg_x = static_cast<int>(gen_reg[nib2]);
        draw_reg_y = static_cast<int>(gen_reg[nib3]);
        call_draw = true;
    }
}

std::uint16_t CPU::get_PC()
{
    return PC;
}

void CPU::update_gen_reg(int regnum, std::uint8_t reg_val){
    gen_reg[regnum] = reg_val;
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
    Display my_display;
    std::uint16_t opcodee{};
    std::array<std::uint8_t, 4> my_arr{};
    my_cpu.load_test_data_ram(0xF11, 0x81);
    my_cpu.load_test_data_ram(0xF12, 0xFF);
    my_cpu.load_test_data_ram(0xF13, 0xc3);
    my_cpu.load_test_data_ram(0xFF1, 0xFF);

    my_cpu.load_test_data_ram(0x200, 0x62);
    my_cpu.load_test_data_ram(0x201, 0x3C);
    my_cpu.load_test_data_ram(0x202, 0x63);
    my_cpu.load_test_data_ram(0x203, 0x1e);

    my_cpu.load_test_data_ram(0x204, 0xAF);
    my_cpu.load_test_data_ram(0x205, 0x11);

    my_cpu.load_test_data_ram(0x206, 0x00);
    my_cpu.load_test_data_ram(0x207, 0xE0);
    my_cpu.load_test_data_ram(0x208, 0xD2);
    my_cpu.load_test_data_ram(0x209, 0x33);

    my_cpu.load_test_data_ram(0x20a, 0xAF);
    my_cpu.load_test_data_ram(0x20b, 0xF1);
    my_cpu.load_test_data_ram(0x20c, 0xD2);
    my_cpu.load_test_data_ram(0x20d, 0x31);

    bool running = true;
    while (running){
        running = my_display.pollevents();
        
        opcodee = my_cpu.fetch();
        my_arr = my_cpu.decode(opcodee);
        my_cpu.execute(my_arr);
        my_display.render_screen();

        if (my_cpu.clear_display == true)
        {
            my_display.clear();
            my_cpu.clear_display = false;
        }

        if (my_cpu.call_draw == true)
        {
            my_display.draw(my_cpu.draw_reg_x, my_cpu.draw_reg_y, my_cpu.bits);
            my_cpu.call_draw = false;
        }
        if (my_display.set_vf == true)
        {
            my_cpu.update_gen_reg(15, 1);
            my_display.set_vf = false;
        } else {
            my_cpu.update_gen_reg(15, 0);
        }

        my_cpu.print_state();
        SDL_Delay(1000);
    }
}

/*int main(){

    Display my_display;
    CPU my_cpu;
    std::uint16_t opcodee{};
    std::array<std::uint8_t, 4> my_arr{};

    my_cpu.load_test_data_ram(0x200, 0x00);
    my_cpu.load_test_data_ram(0x201, 0xE0);

    bool running = true;
    while (running){
        running = my_display.pollevents();

        if (my_cpu.clear_display == true)
        {
            my_display.clear();
            my_cpu.clear_display = false;
        }
        else{
            opcodee = my_cpu.fetch();
            my_arr = my_cpu.decode(opcodee);
            my_cpu.execute(my_arr);
        }
        
        //my_cpu.print_state();
        SDL_Delay(1000);
    }
}*/

/*int main(){

    Display my_display;
    CPU my_cpu;
    std::uint16_t opcodee{};
    std::array<std::uint8_t, 4> my_arr{};

    my_cpu.load_test_data_ram(0x200, 0xF6);
    my_cpu.load_test_data_ram(0x201, 0x0A);

    bool running = true;
    while (running){
        running = my_display.pollevents();

        int the_key = my_display.my_key_val();

        std::cout << '\n' << "wait_reg: " << my_cpu.wait_reg << '\n';

        if (my_cpu.wait_key == true)
        {
            if (the_key != -1)
            {
                my_cpu.update_gen_reg(static_cast<int>(my_cpu.wait_reg), the_key);
                my_cpu.wait_key = false;
            }

        }
        else{
            opcodee = my_cpu.fetch();
            my_arr = my_cpu.decode(opcodee);
            my_cpu.execute(my_arr);
        }
        
        my_cpu.print_state();
        SDL_Delay(1000);
    }
}*/

/*int main(){
    CPU my_cpu;
    std::uint16_t opcodee{};
    std::array<std::uint8_t, 4> my_arr{};

    my_cpu.load_test_data_ram(0x200, 0x63);
    my_cpu.load_test_data_ram(0x201, 0x0D); 
    my_cpu.load_test_data_ram(0x202, 0xF3);
    my_cpu.load_test_data_ram(0x203, 0x55);

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
    
    return 0;
}*/
