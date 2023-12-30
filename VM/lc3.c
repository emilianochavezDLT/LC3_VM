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
u_int16_t memory[MEMORY_SIZE]; //This is the memory of the LC3 VM


int main(){

    printf("Hello World\n");
    return 0;
}
