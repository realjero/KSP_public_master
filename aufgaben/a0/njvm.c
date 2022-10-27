#include <stdio.h>
#include <string.h>

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

// TODO: Instructions berechenen und in program_memory ablegen & ausführen


int sp = 0;
unsigned int stack[MAXITEMS];


unsigned int program_memory[];
int pc;


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
        (PUSHC << 24) | IMMEDIATE(10),
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


void push(int x) {
    stack[sp] = x;
    sp++;
}

int pop() {
    sp--;
    int tmp = stack[sp];
    stack[sp] = 0;
    return tmp;
}

void print_stack(void) {
    printf("\n Stack\n");
    printf(".-------+--------.\n");
    for (int i=sp; i>=0; i--) {
        if (i==sp)
            printf("|sp->%3d| <empty>|\n", i);
        else
            printf("|%7d| %5d |\n", i, stack[i]);
    }
    printf("'-------+--------'\n\n");
}


void show_Code(unsigned int *code) {
    printf("%d", sizeof code);
    for (int i = 0; i <= sizeof(code) + 2; i++) {
        int opcode = code[i] >> 24;

        switch (opcode) {
            case HALT:
                printf("%03d:    halt\n", pc);
                break;
            case PUSHC:
                printf("%03d:    pushc   %d\n", pc, SIGN_EXTEND(code[i] & 0x00FFFFFF));
                break;
            case ADD:
                printf("%03d:    add\n", pc);
                break;
            case SUB:
                printf("%03d:    sub\n", pc);
                break;
            case MUL:
                printf("%03d:    mul\n", pc);
                break;
            case DIV:
                printf("%03d:    div\n", pc);
                break;
            case MOD:
                printf("%03d:    mod\n", pc);
                break;
            case RDINT:
                printf("%03d:    rdint\n", pc);
                break;
            case WRINT:
                printf("%03d:    wrint\n", pc);
                break;
            case RDCHR:
                printf("%03d:    rdchr\n", pc);
                break;
            case WRCHR:
                printf("%03d:    wrchr\n", pc);
                break;
        }

        pc++;
    }
}


int main(int argc, char *argv[]) {
    if(argc == 2) {
        if(strcmp(argv[1], "--help") == 0) {
            printf("usage: ./njvm [option] [option] ...\n");
            printf("  --version        show version and exit\n");
            printf("  --help           show this help and exit\n");
            return 0;
        } else if(strcmp(argv[1], "--version") == 0) {
            printf("Ninja Virtual Machine version 0 (compiled Sep 23 2015, 10:36:52)\n");
            return 0;
        } else if(strcmp(argv[1], "--prog1") == 0) {
            printf("Ninja Virtual Machine started\n");
            show_Code(code1);
            printf("Ninja Virtual Machine stopped\n");
            return 0;
        } else if(strcmp(argv[1], "--prog2") == 0) {
            printf("Ninja Virtual Machine started\n");
            show_Code(code2);
            printf("Ninja Virtual Machine stopped\n");
            return 0;
        } else if(strcmp(argv[1], "--prog3") == 0) {
            printf("Ninja Virtual Machine started\n");
            show_Code(code3);
            printf("Ninja Virtual Machine stopped\n");
            return 0;
        } else {
            printf("unknown command line argument '%s', try './njvm --help'\n", argv[1]);
            return 0;
        }
    }

    return 0;
}