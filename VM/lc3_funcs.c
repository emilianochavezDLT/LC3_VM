#include <lc3_funcs.h>
#include "../registers/registers.h"
#include "../instructions/opcodes.h"
#include "../flags/flags.h"




void update_flags(uint16_t r, uint16_t reg[]){
    
    if(reg[r] == 0){
        reg[R_COND] = FL_ZRO;
    }
    else if(reg[r] >> 15){ // a 1 in the left most bit indicates negative
        reg[R_COND] = FL_NEG;
    }
    else{
        reg[R_COND] = FL_POS;
    }   
}

uint16_t sign_extend(uint16_t x, int bit_count){
    if((x >> (bit_count - 1)) & 1){
        x |= (0xFFFF << bit_count);
    }
    return x;
}

void read_image_file(FILE* file, uint16_t memory[]){
    uint16_t origin;
    fread(&origin, sizeof(origin), 1, file);
    origin = swap16(origin);
    
    uint16_t max_read = UINT16_MAX - origin;
    uint16_t* p = memory + origin;
    
    size_t read = fread(p, sizeof(uint16_t), max_read, file);
    
    while(read-- > 0){
        *p = swap16(*p);
        ++p;
    }
}

uint16_t swap16(uint16_t x){
    return (x << 8) | (x >> 8);
}

int read_image(const char* image_path, uint16_t memory[]){
    FILE* file = fopen(image_path, "rb");
    if(!file){
        return 0;
    }
    read_image_file(file, memory);
    fclose(file);
    return 1;
}

void mem_write(uint16_t address, uint16_t val, uint16_t memory[]){
    memory[address] = val;
}

uint16_t mem_read(uint16_t address, uint16_t memory[]){
    if(address == MR_KBSR){
        if(check_key()){
            memory[MR_KBSR] = (1 << 15);
            memory[MR_KBDR] = getchar();
        }
        else{
            memory[MR_KBSR] = 0;
        }
    }
    return memory[address];
}

void disable_input_buffering(){
    tcgetattr(STDIN_FILENO, &original_tio);
    struct termios new_tio = original_tio;
    new_tio.c_lflag &= ~ICANON & ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
}

void restore_input_buffering(){
    tcsetattr(STDIN_FILENO, TCSANOW, &original_tio);
}


uint16_t check_key(){
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    return select(1, &readfds, NULL, NULL, &timeout) != 0;
}

void handle_interrupt(int signal){
    restore_input_buffering();
    printf("\n");
    exit(-2);
}