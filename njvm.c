#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// gcc -g -Wall -std=c99 -pedantic -o vm njvm.c

#define MAXITEMS 100

// INSTRUCTIONS

#define HALT 0
#define PUSHC 1
#define ADD 2
#define SUB 3
#define MUL 4
#define DIV 5
#define MOD 6
#define RDINT 7
#define WRINT 8
#define RDCHR 9
#define WRCHR 10

// CONVERT

#define IMMEDIATE(x) ((x) & 0x00FFFFFF)                                     // Zahl unter 23 Bit
#define SIGN_EXTEND(i) ((i) & 0x00800000 ? (i) | 0xFF000000 : (i))          // Wenn Bit 23 => 1, dann negative Zahl, die durch 8 weitere 1-en in den höchstwertigen Bits erweitert werden muss.


int stack_pointer = 0;
unsigned int stack[MAXITEMS];

unsigned int *program_memory;
int program_counter;

// CODE FILES
/*
//     writeInteger((3 + 4) * (10 - 6));
//     writeCharacter('\n');
unsigned int code1[] = {
        (PUSHC << 24) | IMMEDIATE(3),
        (PUSHC << 24) | IMMEDIATE(4),
        (ADD << 24),
        (PUSHC << 24) | IMMEDIATE(10),
        (PUSHC << 24) | IMMEDIATE(6),
        (SUB << 24),
        (MUL << 24),
        (WRINT << 24),
        (PUSHC << 24) | IMMEDIATE('\n'),
        (WRCHR << 24),
        (HALT << 24)
};

//    writeInteger(-2 * readInteger() + 3);
//    writeCharacter('\n');
unsigned int code2[] = {
        (PUSHC << 24) | IMMEDIATE(-2),
        (RDINT << 24),
        (MUL << 24),
        (PUSHC << 24) | IMMEDIATE(3),
        (MUL << 24),
        (WRINT << 24),
        (PUSHC << 24) | IMMEDIATE('\n'),
        (WRCHR << 24),
        (HALT << 24)
};

//    writeInteger(readCharacter());
//    writeCharacter('\n');
unsigned int code3[] = {
        (RDCHR << 24),
        (WRINT << 24),
        (PUSHC << 24) | IMMEDIATE('\n'),
        (WRCHR << 24),
        (HALT << 24)
};
*/

void push(int x) {
    if(stack_pointer <= MAXITEMS) {
        stack[stack_pointer] = x;
        stack_pointer++;
    } else {
        printf("Error: Stackoverflow\n");
        exit(0);
    }
}

int pop() {
    if(stack_pointer > 0) {
        stack_pointer--;
        int tmp = stack[stack_pointer];
        stack[stack_pointer] = 0;
        return tmp;
    } else {
        printf("Error: Stack empty\n");
        exit(0);
    }
}


void print_program_memory(void) {
    int opcode;
    do {
        opcode = program_memory[program_counter] >> 24;
        switch (opcode) {
            case HALT:
                printf("%03d:\thalt\n", program_counter);
                break;
            case PUSHC:
                printf("%03d:\tpushc\t%d\n", program_counter, SIGN_EXTEND(program_memory[program_counter] & 0x00FFFFFF));
                break;
            case ADD:
                printf("%03d:\tadd\n", program_counter);
                break;
            case SUB:
                printf("%03d:\tsub\n", program_counter);
                break;
            case MUL:
                printf("%03d:\tmul\n", program_counter);
                break;
            case DIV:
                printf("%03d:\tdiv\n", program_counter);
                break;
            case MOD:
                printf("%03d:\tmod\n", program_counter);
                break;
            case RDINT:
                printf("%03d:\trdint\n", program_counter);
                break;
            case WRINT:
                printf("%03d:\twrint\n", program_counter);
                break;
            case RDCHR:
                printf("%03d:\trdchr\n", program_counter);
                break;
            case WRCHR:
                printf("%03d:\twrchr\n", program_counter);
                break;
        }

        program_counter++;
    } while (opcode != HALT);
    program_counter = 0;
}

void start_program_memory(void) {
    int opcode;
    do {
        int value;
        char c;
        int x, y;

        opcode = program_memory[program_counter] >> 24;
        switch (opcode) {
            case HALT:
                break;
            case PUSHC:
                push(SIGN_EXTEND(program_memory[program_counter] & 0x00FFFFFF));
                break;
            case ADD:
                y = pop();
                x = pop();
                push(x + y);
                break;
            case SUB:
                y = pop();
                x = pop();
                push(x - y);
                break;
            case MUL:
                y = pop();
                x = pop();
                push(x * y);
                break;
            case DIV:
                y = pop();
                x = pop();
                if(y != 0) {
                    push(x / y);
                } else {
                    printf("Error: Division by zero\n");
                    exit(0);
                }
                break;
            case MOD:
                y = pop();
                x = pop();
                if(y != 0) {
                    push(x % y);
                } else {
                    printf("Error: Modulo by zero\n");
                    exit(0);
                }
                break;
            case RDINT:
                scanf("%d", &value);
                push(value);
                break;
            case WRINT:
                printf("%d", pop());
                break;
            case RDCHR:
                scanf("%c", &c);
                push(c);
                break;
            case WRCHR:
                printf("%c", pop());
                break;
        }

        program_counter++;
    } while (opcode != HALT);
    program_counter = 0;
}

int main(int argc, char *argv[]) {
    if(argc == 2) {
        if(argv[1][0] == '-') {
            if (strcmp(argv[1], "--help") == 0) {
                printf("usage: ./njvm [option] [option] ...\n");
                printf("  --version        show version and exit\n");
                printf("  --help           show this help and exit\n");
                return 0;
            } else if (strcmp(argv[1], "--version") == 0) {
                printf("Ninja Virtual Machine version 0 (compiled Sep 23 2015, 10:36:52)\n");
                return 0;
            } else {
                printf("unknown command line argument '%s', try './njvm --help'\n", argv[1]);
                return 0;
            }
        } else if(strcmp(argv[1], "") == 0){
            printf("Error: no code file specified\n");
            return 0;
        } else {
            FILE *fp = NULL;
            fp = fopen(argv[1], "r");
            if(fp == NULL) {
                printf("Error: cannot open code file '%s'", argv[1]);
                return 0;
            }
            char c[4];
            size_t read_len = fread(c, 1, 4, fp);
            printf("r %d bytes: c = [%c, %c, %c, %c]\n", read_len, c[0], c[1], c[2], c[3]);
        }
    }

    if(program_memory != NULL) {
        printf("Ninja Virtual Machine started\n");

        print_program_memory();
        start_program_memory();

        printf("Ninja Virtual Machine stopped\n");
    } else  {
        printf("Error: no program selected\n");
    }

    return 0;
}