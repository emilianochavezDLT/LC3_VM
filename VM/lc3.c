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
#include "../traps/traps.h"
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/termios.h>
#include <sys/mman.h>


//This represents the number of memory locations that the LC3 VM has
#define MEMORY_SIZE (1 << 16) // 2^16 or 65536, which in english is moving the bits 16 times to the left
u_int16_t memory[MEMORY_SIZE]; //This is the memory of the LC3 VM and we are storing it in an array.



u_int16_t reg[R_COUNT]; //This is the array of registers


int main(int argc, const char* argv[]){

    signal(SIGINT, handle_interrupt);
    disable_input_buffering();


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
                uint16_t r0 = (instr >> 9) & 0x7; //We are shifting the bits 9 times to the right and then we are ANDing it with 0x7
                uint16_t r1 = (instr >> 6) & 0x7; //We are shifting the bits 6 times to the right and then we are ANDing it with 0x7
                uint16_t imm_flag = (instr >> 5) & 0x1; //We are shifting the bits 5 times to the right and then we are ANDing it with 0x1

                if(imm_flag){
                    uint16_t imm5 = sign_extend(instr & 0x1F, 5); //We are ANDing it with 0x1F and then we are sign extending it by 5 bits
                    reg[r0] = reg[r1] & imm5;
                }
                else{
                    uint16_t r2 = instr & 0x7; //We are ANDing it with 0x7
                    reg[r0] = reg[r1] & reg[r2];
                }
                update_flags(r0, reg);
                break;
            case OP_NOT:
                uint16_t r0 = (instr >> 9) & 0x7; //We are shifting the bits 9 times to the right and then we are ANDing it with 0x7
                uint16_t r1 = (instr >> 6) & 0x7; //We are shifting the bits 6 times to the right and then we are ANDing it with 0x7
                reg[r0] = ~reg[r1];
                update_flags(r0, reg);
                break;
            case OP_BR:
                uint16_t pc_offset = sign_extend(instr & 0x1FF, 9); //We are ANDing it with 0x1FF and then we are sign extending it by 9 bits
                uint16_t cond_flag = (instr >> 9) & 0x7; //We are shifting the bits 9 times to the right and then we are ANDing it with 0x7
                if(cond_flag & reg[R_COND]){
                    reg[R_PC] += pc_offset;
                }
                break;
            case OP_JMP:
                uint16_t r1 = (instr >> 6) & 0x7; //We are shifting the bits 6 times to the right and then we are ANDing it with 0x7
                reg[R_PC] = reg[r1];
                break;
            case OP_JSR:
                uint16_t long_flag = (instr >> 11) & 1; //We are shifting the bits 11 times to the right and then we are ANDing it with 0x1
                reg[R_R7] = reg[R_PC];
                if(long_flag){
                    uint16_t long_pc_offset = sign_extend(instr & 0x7FF, 11); //We are ANDing it with 0x7FF and then we are sign extending it by 11 bits
                    reg[R_PC] += long_pc_offset; 
                }
                else{
                    uint16_t r1 = (instr >> 6) & 0x7; //We are shifting the bits 6 times to the right and then we are ANDing it with 0x7
                    reg[R_PC] = reg[r1];
                }
                break;
            case OP_LD:
                /*destination register (DR)*/
                uint16_t r0 = (instr >> 9) & 0x7; //We are shifting the bits 9 times to the right and then we are ANDing it with 0x7
                /*PCoffset 9*/
                uint16_t pc_offset = sign_extend(instr & 0x1FF, 9); //We are ANDing it with 0x1FF and then we are sign extending it by 9 bits
                /*Add pc_offset to the current PC, look at that memory location to get the final address*/
                reg[r0] = mem_read(reg[R_PC] + pc_offset);
                update_flags(r0, reg);
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
                /*destination register (DR)*/
                uint16_t r0 = (instr >> 9) & 0x7; //We are shifting the bits 9 times to the right and then we are ANDing it with 0x7
                /*Base register (BR)*/
                uint16_t r1 = (instr >> 6) & 0x7; //We are shifting the bits 6 times to the right and then we are ANDing it with 0x7
                /*Offset 6*/
                uint16_t offset = sign_extend(instr & 0x3F, 6); //We are ANDing it with 0x3F and then we are sign extending it by 6 bits
                /*Add offset to the current PC, look at that memory location to get the final address*/
                reg[r0] = mem_read(reg[r1] + offset);
                update_flags(r0, reg);
                break;
            case OP_LEA:
                /*destination register (DR)*/
                uint16_t r0 = (instr >> 9) & 0x7; //We are shifting the bits 9 times to the right and then we are ANDing it with 0x7
                /*PCoffset 9*/
                uint16_t pc_offset = sign_extend(instr & 0x1FF, 9); //We are ANDing it with 0x1FF and then we are sign extending it by 9 bits
                /*Add pc_offset to the current PC, look at that memory location to get the final address*/
                reg[r0] = reg[R_PC] + pc_offset;
                update_flags(r0, reg);
                break;
            case OP_ST:
                uint16_t r0 = (instr >> 9) & 0x7; //We are shifting the bits 9 times to the right and then we are ANDing it with 0x7
                uint16_t pc_offset = sign_extend(instr & 0x1FF, 9); //We are ANDing it with 0x1FF and then we are sign extending it by 9 bits
                mem_write(reg[R_PC] + pc_offset, reg[r0]);
                break;
            case OP_STI:
                uint16_t r0 = (instr >> 9) & 0x7; //We are shifting the bits 9 times to the right and then we are ANDing it with 0x7
                uint16_t pc_offset = sign_extend(instr & 0x1FF, 9); //We are ANDing it with 0x1FF and then we are sign extending it by 9 bits
                mem_write(mem_read(reg[R_PC] + pc_offset), reg[r0]);
                break;
            case OP_STR:
                uint16_t r0 = (instr >> 9) & 0x7; //We are shifting the bits 9 times to the right and then we are ANDing it with 0x7
                uint16_t r1 = (instr >> 6) & 0x7; //We are shifting the bits 6 times to the right and then we are ANDing it with 0x7
                uint16_t offset = sign_extend(instr & 0x3F, 6); //We are ANDing it with 0x3F and then we are sign extending it by 6 bits
                mem_write(reg[r1] + offset, reg[r0]);
                break;
            case OP_TRAP:   
                switch(instr & 0xFF){
                    case TRAP_GETC:
                        reg[R_R0] = (uint16_t)getchar();
                        break;
                    case TRAP_OUT:
                        putc((char)reg[R_R0], stdout);
                        fflush(stdout);
                        break;
                    case TRAP_PUTS:
                        {
                            uint16_t* c = memory + reg[R_R0];
                            while(*c){
                                putc((char)*c, stdout);
                                ++c;
                            }
                            fflush(stdout);
                        }
                        break;
                    case TRAP_IN:
                        {
                            printf("Enter a character: ");
                            char c = getchar();
                            putc(c, stdout);
                            reg[R_R0] = (uint16_t)c;
                        }
                        break;
                    case TRAP_PUTSP:
                        {
                            uint16_t* c = memory + reg[R_R0];
                            while(*c){
                                char char1 = (*c) & 0xFF;
                                putc(char1, stdout);
                                char char2 = (*c) >> 8;
                                if(char2) putc(char2, stdout);
                                ++c;
                            }
                            fflush(stdout);
                        }
                        break;
                    case TRAP_HALT:
                        puts("HALT");
                        fflush(stdout);
                        running = 0;
                        break;
                }
                break;
            default:
                abort();
                break;

        }

    }    
    restore_input_buffering();
    return 0;
}
