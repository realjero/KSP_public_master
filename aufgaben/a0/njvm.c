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
int stack[MAXITEMS];

unsigned int program_memory[];
int mp;


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
        } else {
            printf("unknown command line argument '%s', try './njvm --help'\n", argv[1]);
            return 0;
        }
    }

    printf("Ninja Virtual Machine started\n");

    printf("%d\n", IMMEDIATE(0x00FFFFFF));

    printf("Ninja Virtual Machine stopped\n");
    return 0;
}