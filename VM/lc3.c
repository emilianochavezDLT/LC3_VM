//Emiliano Chavez De La Torre
/* 
This is the main file for the LC3 Virtual Machine. 
Creating this LC3 VM is a learning expereince. 
Im follwing the tortuial set by https://www.jmeiners.com/lc3-vm/#s0:0
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

//This represents the number of memory locations that the LC3 VM has
#define MEMORY_SIZE (1 << 16) // 2^16 or 65536, which in english is moving the bits 16 times to the left
u_int16_t memory[MEMORY_SIZE]; //This is the memory of the LC3 VM and we are storing it in an array.

enum {
    R_R0 = 0,
    R_R1,
    R_R2,
    R_R3,
    R_R4,
    R_R5,
    R_R6,
    R_R7,
    R_PC, //Program Counter
    R_COND, //Condition Flag
    R_COUNT //This is the number of registers
};

u_int16_t reg[R_COUNT]; //This is the array of registers

enum
{
    OP_BR = 0, /* branch */
    OP_ADD,    /* add  */
    OP_LD,     /* load */
    OP_ST,     /* store */
    OP_JSR,    /* jump register */
    OP_AND,    /* bitwise and */
    OP_LDR,    /* load register */
    OP_STR,    /* store register */
    OP_RTI,    /* unused */
    OP_NOT,    /* bitwise not */
    OP_LDI,    /* load indirect */
    OP_STI,    /* store indirect */
    OP_JMP,    /* jump */
    OP_RES,    /* reserved (unused) */
    OP_LEA,    /* load effective address */
    OP_TRAP    /* execute trap */
};

enum
{
    FL_POS = 1 << 0, /* Positive Flag */
    FL_ZRO = 1 << 1, /* Zero Flag */
    FL_NEG = 1 << 2, /* Negative Flag */
};


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
