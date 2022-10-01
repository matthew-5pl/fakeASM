#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <string.h>
#include <time.h>

#define PROGRAM_MAX_CHAR 10000

typedef enum {
    CONTINUE,
    END,
    RET
} processor_status_t;

typedef enum {
    RA,
    RB,
    RC,
    RAX
} processor_reg_t;

typedef struct machine {
    void (*pc)(struct machine*);
    uint8_t stack[255];
    processor_status_t status;  // processor status
    uint8_t a;                  // register a
    uint8_t b;                  // register b
    uint8_t c;                  // register c
    uint8_t ax[255];            // array register
    uint8_t cnt;                // array counter
    uint8_t la;                 // latest mov
    uint8_t lp;                 // latest push-pop
    void (*prg)(struct machine*);
} machine;

typedef void (*program_t)(machine*);

processor_status_t mov(machine* m, uint8_t value, processor_reg_t reg);

uint8_t push(machine* m, uint8_t value);

processor_status_t ret(machine* m);

uint8_t pop(machine* m);

machine* init_machine(program_t prg);

void program_run(machine* m);

processor_status_t jmp(machine* m, program_t prog);

processor_status_t jnz(machine* m, program_t prog);

void parse(machine* m, char* buf);

char* timeandate();