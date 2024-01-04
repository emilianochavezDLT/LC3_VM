//Emiliano Chavez De La Torre
/* 
This is the main file for the LC3 Virtual Machine. 
Creating this LC3 VM is a learning expereince. 
Im follwing the tortuial set by https://www.jmeiners.com/lc3-vm/#s0:0
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../registers/registers.h"
#include "../instructions/opcodes.h"
#include "../flags/flags.h"

//This represents the number of memory locations that the LC3 VM has
#define MEMORY_SIZE (1 << 16) // 2^16 or 65536, which in english is moving the bits 16 times to the left
u_int16_t memory[MEMORY_SIZE]; //This is the memory of the LC3 VM and we are storing it in an array.



u_int16_t reg[R_COUNT]; //This is the array of registers


int main(int argc, const char* argv[]){

    /*There should be only one condition flag set at any given time.
     By default we are setting the zero flag*/
    reg[R_COND] = FL_ZRO;

    /*We will next set up the PC starting position.
    0x3000 is the default adress that we start on.*/
    enum { PC_START = 0x3000 };
    reg[R_PC] = PC_START;

    /*We will now load the arguments into the VM memory*/
    if (argc < 2)
    {
        /*Show usage string and exit*/
        printf("lc3 [image-file1] ...\n");
        exit(2);
    }

    for (int j = 1; j < argc; ++j)
    {
        if (!read_image(argv[j]))
        {
            printf("failed to load image: %s\n", argv[j]);
            exit(1);
        }
    }

    int running = 1;
    while (running)
    {
        /*Fetch*/
        uint16_t instr = mem_read(reg[R_PC]++);

        uint16_t op = instr >> 12; //We are shifting the bits 12 times to the right
        switch (op)
        {
            case OP_ADD:
                /*Destination register (DR)*/
                uint16_t r0 = (instr >> 9) & 0x7; //We are shifting the bits 9 times to the right and then we are ANDing it with 0x7
                /*First operand (SR1)*/
                uint16_t r1 = (instr >> 6) & 0x7; //We are shifting the bits 6 times to the right and then we are ANDing it with 0x7
                /*Check if its an immediate mode*/
                uint16_t imm_flag = (instr >> 5) & 0x1; //We are shifting the bits 5 times to the right and then we are ANDing it with 0x1

                if(imm_flag){
                    uint16_t imm5 = sign_extend(instr & 0x1F, 5); //We are ANDing it with 0x1F and then we are sign extending it by 5 bits
                    reg[r0] = reg[r1] + imm5;
                }
                else{
                    uint16_t r2 = instr & 0x7; //We are ANDing it with 0x7
                    reg[r0] = reg[r1] + reg[r2];
                }
                update_flags(r0, reg);

                break;
            case OP_AND:
                break;
            case OP_NOT:
                break;
            case OP_BR:
                break;
            case OP_JMP:
                break;
            case OP_JSR:
                break;
            case OP_LD:
                break;
            case OP_LDI:
                
                /*destination register (DR)*/
                uint16_t r0 = (instr >> 9) & 0x7; //We are shifting the bits 9 times to the right and then we are ANDing it with 0x7
                /*PCoffset 9*/
                uint16_t pc_offset = sign_extend(instr & 0x1FF, 9); //We are ANDing it with 0x1FF and then we are sign extending it by 9 bits
                /*Add pc_offset to the current PC, look at that memory location to get the final address*/
                reg[r0] = mem_read(mem_read(reg[R_PC] + pc_offset));
                update_flags(r0, reg);

                break;
            case OP_LDR:
                break;
            case OP_LEA:
                break;
            case OP_ST:
                break;
            case OP_STI:
                break;
            case OP_STR:
                break;
            case OP_TRAP:   
                break;
            case OP_RES:
            case OP_RTI:
            default:
                abort();
                break;

        }

    }    
    return 0;
}
