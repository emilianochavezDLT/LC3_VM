#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <lc3_funcs.h>
#include "../registers/registers.h"
#include "../instructions/opcodes.h"
#include "../flags/flags.h"

//Before you do anything, you need to rearrange the file system
//So that you can access the opcodes and the registers from a header file
//Call one opcode and the other registers for example. OR you can just store them in one header file.

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