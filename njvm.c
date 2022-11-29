#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// gcc -g -Wall -std=c99 -pedantic -o vm njvm.c

#define MAXITEMS 10000

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
#define PUSHG 11
#define POPG 12
#define ASF 13
#define RSF 14
#define PUSHL 15
#define POPL 16
#define EQ 17
#define NE 18
#define LT 19
#define LE 20
#define GT 21
#define GE 22
#define JMP 23
#define BRF 24
#define BRT 25


//TODO: CATCH NEGATIVES, IMMEDIATE INSTEAD OF SIGN EXTEND

// CONVERT
#define IMMEDIATE(x) ((x) & 0x00FFFFFF)                                     // Zahl unter 23 Bit
#define SIGN_EXTEND(i) ((i) & 0x00800000 ? (i) | 0xFF000000 : (i))          // Wenn Bit 23 => 1, dann negative Zahl, die durch 8 weitere 1-en in den höchstwertigen Bits erweitert werden muss.


int stack_pointer = 0;
int frame_pointer = 0;
unsigned int stack[MAXITEMS];

int sda_size = 0;
unsigned int *sda;

unsigned int *program_memory;
int program_counter;


void push_stack(int x) {
    if(stack_pointer == MAXITEMS) {
        printf("Error: Stackoverflow\n");
        exit(0);
    }
    stack[stack_pointer] = x;
    stack_pointer++;
}

int pop_stack(void) {
    if(stack_pointer == 0) {
        printf("Error: Stack empty\n");
        exit(0);
    }
    stack_pointer--;
    int tmp = stack[stack_pointer];
    stack[stack_pointer] = 0;
    return tmp;
}


void print_program_memory(void) {
    int instr;
    do {
        instr = program_memory[program_counter];
        program_counter++;

        switch (instr >> 24) {
            case HALT:
                printf("%03d:\thalt\n", program_counter - 1);
                break;
            case PUSHC:
                printf("%03d:\tpushc\t%d\n", program_counter - 1, SIGN_EXTEND(IMMEDIATE(instr)));
                break;
            case ADD:
                printf("%03d:\tadd\n", program_counter - 1);
                break;
            case SUB:
                printf("%03d:\tsub\n", program_counter - 1);
                break;
            case MUL:
                printf("%03d:\tmul\n", program_counter - 1);
                break;
            case DIV:
                printf("%03d:\tdiv\n", program_counter - 1);
                break;
            case MOD:
                printf("%03d:\tmod\n", program_counter - 1);
                break;
            case RDINT:
                printf("%03d:\trdint\n", program_counter - 1);
                break;
            case WRINT:
                printf("%03d:\twrint\n", program_counter - 1);
                break;
            case RDCHR:
                printf("%03d:\trdchr\n", program_counter - 1);
                break;
            case WRCHR:
                printf("%03d:\twrchr\n", program_counter - 1);
                break;
            case PUSHG:
                printf("%03d:\tpushg\t%d\n", program_counter - 1, IMMEDIATE(instr));
                break;
            case POPG:
                printf("%03d:\tpopg\t%d\n", program_counter - 1, IMMEDIATE(instr));
                break;
            case ASF:
                printf("%03d:\tasf\t%d\n", program_counter - 1, IMMEDIATE(instr));
                break;
            case RSF:
                printf("%03d:\trsf\n", program_counter - 1);
                break;
            case PUSHL:
                printf("%03d:\tpushl\t%d\n", program_counter - 1, IMMEDIATE(instr));
                break;
            case POPL:
                printf("%03d:\tpopl\t%d\n", program_counter - 1, IMMEDIATE(instr));
                break;
            case EQ:
                printf("%03d:\teq\n", program_counter - 1);
                break;
            case NE:
                printf("%03d:\tne\n", program_counter - 1);
                break;
            case LT:
                printf("%03d:\tlt\n", program_counter - 1);
                break;
            case LE:
                printf("%03d:\tle\n", program_counter - 1);
                break;
            case GT:
                printf("%03d:\tgt\n", program_counter - 1);
                break;
            case GE:
                printf("%03d:\tge\n", program_counter - 1);
                break;
            case JMP:
                printf("%03d:\tjmp\t%d\n", program_counter - 1, IMMEDIATE(instr));
                break;
            case BRF:
                printf("%03d:\tbrf\t%d\n", program_counter - 1, IMMEDIATE(instr));
                break;
            case BRT:
                printf("%03d:\tbrt\t%d\n", program_counter - 1, IMMEDIATE(instr));
                break;
        }

        // Anzeigen des Stacks
        // Anzeigen der statischen Daten
        // Listen des Programms
        // Ausführen des nächsten Befehls
        // Weiterlaufen ohne Anhalten und Verlassen
        // ? BREAKPOINTS ?

    } while (instr >> 24 != HALT);
    program_counter = 0;
}

void start_program_memory(void) {
    int instr;
    do {
        int value;
        char c;
        int x, y;

        instr = program_memory[program_counter];
        program_counter++;

        switch (instr >> 24) {
            case HALT:
                break;
            case PUSHC:
                push_stack(SIGN_EXTEND(instr & 0x00FFFFFF));
                break;
            case ADD:
                y = pop_stack();
                x = pop_stack();
                push_stack(x + y);
                break;
            case SUB:
                y = pop_stack();
                x = pop_stack();
                push_stack(x - y);
                break;
            case MUL:
                y = pop_stack();
                x = pop_stack();
                push_stack(x * y);
                break;
            case DIV:
                y = pop_stack();
                x = pop_stack();
                if(y == 0) {
                    printf("Error: Division by zero\n");
                    exit(0);
                }
                push_stack(x / y);
                break;
            case MOD:
                y = pop_stack();
                x = pop_stack();
                if(y == 0) {
                    printf("Error: Modulo by zero\n");
                    exit(0);
                }
                push_stack(x % y);
                break;
            case RDINT:
                scanf("%d", &value);
                push_stack(value);
                break;
            case WRINT:
                printf("%d", pop_stack());
                break;
            case RDCHR:
                scanf("%c", &c);
                push_stack(c);
                break;
            case WRCHR:
                printf("%c", pop_stack());
                break;
            case PUSHG:
                // Das n-te Element der SDA wird auf dem Stack abgelegt
                push_stack(sda[IMMEDIATE(instr)]);
                break;
            case POPG:
                // Der Wert value wird in der SDA als n-tes Element gespeichert
                sda[IMMEDIATE(instr)] = pop_stack();
                break;
            case ASF:
                push_stack(frame_pointer); // save current fp on stack
                frame_pointer = stack_pointer; // set start of frame
                stack_pointer = stack_pointer + IMMEDIATE(instr); // allocate n vars in frame
                break;
            case RSF:
                stack_pointer = frame_pointer; // points to old fp value
                frame_pointer = pop_stack();   // set fp to old value
                break;
            case PUSHL:
                push_stack(stack[frame_pointer + IMMEDIATE(instr)]);
                break;
            case POPL:
                stack[frame_pointer + IMMEDIATE(instr)] = pop_stack();
                break;
            case EQ:
                y = pop_stack();
                x = pop_stack();
                push_stack(x == y ? 1 : 0);
                break;
            case NE:
                y = pop_stack();
                x = pop_stack();
                push_stack(x != y ? 1 : 0);
                break;
            case LT:
                y = pop_stack();
                x = pop_stack();
                push_stack(x < y ? 1 : 0);
                break;
            case LE:
                y = pop_stack();
                x = pop_stack();
                push_stack(x <= y ? 1 : 0);
                break;
            case GT:
                y = pop_stack();
                x = pop_stack();
                push_stack(x > y ? 1 : 0);
                break;
            case GE:
                y = pop_stack();
                x = pop_stack();
                push_stack(x >= y ? 1 : 0);
                break;
            case JMP:
                program_counter = IMMEDIATE(instr);
                break;
            case BRF:
                if(pop_stack() == 0) program_counter = IMMEDIATE(instr);
                break;
            case BRT:
                if(pop_stack() == 1) program_counter = IMMEDIATE(instr);
                break;
        }
    } while (instr >> 24 != HALT);
    program_counter = 0;
}

void command_line_arguments(int argc, char *argv[]) {
    if(argc == 2) {
        // COMMAND LINE ARGUMENTS
        if(argv[1][0] == '-') {
            if (strcmp(argv[1], "--help") == 0) {
                printf("usage: ./njvm [option] [option] ...\n");
                printf("  --version        show version and exit\n");
                printf("  --help           show this help and exit\n");
                exit(0);
            } else if (strcmp(argv[1], "--version") == 0) {
                printf("Ninja Virtual Machine version 2 (compiled Sep 23 2015, 10:36:52)\n");
                exit(0);
            } else {
                printf("unknown command line argument '%s', try './njvm --help'\n", argv[1]);
                exit(0);
            }
        }

        // CODE LOADING
        if(strcmp(argv[1], "") == 0){
            printf("Error: no code file specified\n");
            exit(0);
        }

        FILE *f = NULL;
        if((f = fopen(argv[1], "r")) == NULL) {
            printf("Error: cannot open code file '%s'", argv[1]);
            exit(0);
        }

        // READ FIRST 4 LINES
        // READ NJBF // READ VERSION // READ NUMBER OF INSTR // READ NUMBER OF SDA
        unsigned int format[4];
        fread(format, sizeof (unsigned int), 4, f);
        if((format[0] != 1178749518) | (format[1] != 3)) {
            printf("error bin not matching");
            exit(0);
        }

        // ALLOCATE MEMORY
        sda_size = format[3];
        sda = (unsigned int *) malloc(sda_size*sizeof (unsigned int));
        program_memory = (unsigned int *) malloc(format[2]*sizeof (unsigned int));
        fread(program_memory, sizeof (unsigned int), format[2], f);
        fclose(f);

        // RUN PROGRAM
        printf("Ninja Virtual Machine started\n");
        //print_program_memory();
        start_program_memory();
        printf("Ninja Virtual Machine stopped\n");

        free(sda);
        free(program_memory);
        exit(0);

    }
}

int main(int argc, char *argv[]) {
    command_line_arguments(argc, argv);
    return 0;
}