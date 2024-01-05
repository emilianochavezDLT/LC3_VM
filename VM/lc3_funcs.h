#ifndef _LC3_FUNCS_H
#define _LC3_FUNCS_H


#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/termios.h>
#include <sys/mman.h>


enum{
    MR_KBSR = 0xFE00, // keyboard status
    MR_KBDR = 0xFE02 // keyboard data
};

struct termios original_tio;

//The update flags function will update the condition flags
void update_flags(uint16_t r, uint16_t reg[]);

uint16_t sign_extend(uint16_t x, int bit_count);

void read_image_file(FILE* file, uint16_t memory[]);

uint16_t swap16(uint16_t x);

int read_image(const char* image_path, uint16_t memory[]);

void mem_write(uint16_t address, uint16_t val, uint16_t memory[]);

uint16_t mem_read(uint16_t address, uint16_t memory[]);

void disable_input_buffering();

void restore_input_buffering();

void handle_interrupt(int signal);


#endif