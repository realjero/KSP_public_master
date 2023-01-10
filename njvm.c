#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

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
#define CALL 26
#define RET 27
#define DROP 28
#define PUSHR 29
#define POPR 30
#define DUP 31


//TODO: CATCH NEGATIVES, IMMEDIATE INSTEAD OF SIGN EXTEND, BREAKPOINTS

// CONVERT
#define IMMEDIATE(x) ((x) & 0x00FFFFFF)                                     // Zahl <= 23 Bit
#define SIGN_EXTEND(i) ((i) & 0x00800000 ? (i) | 0xFF000000 : (i))          // Wenn Bit 23 => 1, dann negative Zahl, die durch 8 weitere 1-en in den höchstwertigen Bits erweitert werden muss.

typedef struct {
    unsigned int size;
    unsigned char data[1];
} *ObjRef;

typedef struct {
    bool isObjRef;
    union {
        ObjRef objRef;
        int number;
    } u;
} StackSlot;

int stack_pointer = 0;
int frame_pointer = 0;
StackSlot stack[MAXITEMS];

ObjRef *sda;
size_t sda_size;

ObjRef return_value_register;

unsigned int *program_memory;
int program_counter;

void print_sda() {
    for (int i = 0; i < sda_size; i++) {
        printf("data[%04d]:\t\t%d\n", i, sda[i]);
    }
    printf("\t\t\t\t--- end of data ---\n");
}

void print_stack() {
    for (int i = stack_pointer; i >= 0; i--) {
        if(i == stack_pointer) {
            printf("sp\t\t--->\t%04d:\txxxx\n", i);
        } else if (i == frame_pointer) {
            if(stack[i].isObjRef == true) {
                printf("fp\t\t--->\t%04d:\t%p (objref)\n", i, stack[i].u.objRef);
            } else {
                printf("fp\t\t--->\t%04d:\t%d (number)\n", i, stack[i].u.number);
            }
        } else {
            if(stack[i].isObjRef == true) {
                printf("\t\t\t\t%04d:\t%p (objref)\n", i, stack[i].u.objRef);
            } else {
                printf("\t\t\t\t%04d:\t%d\n (number)", i, stack[i].u.number);
            }
        }
    }
    printf("\t\t\t\t\t\t--- bottom of stack ---\n");
}

void print_instruction(unsigned int instr, int pc) {
    switch (instr >> 24) {
        case HALT:
            printf("%03d:\thalt\n", pc);
            break;
        case PUSHC:
            printf("%03d:\tpushc\t%d\n", pc, SIGN_EXTEND(IMMEDIATE(instr)));
            break;
        case ADD:
            printf("%03d:\tadd\n", pc);
            break;
        case SUB:
            printf("%03d:\tsub\n", pc);
            break;
        case MUL:
            printf("%03d:\tmul\n", pc);
            break;
        case DIV:
            printf("%03d:\tdiv\n", pc);
            break;
        case MOD:
            printf("%03d:\tmod\n", pc);
            break;
        case RDINT:
            printf("%03d:\trdint\n", pc);
            break;
        case WRINT:
            printf("%03d:\twrint\n", pc);
            break;
        case RDCHR:
            printf("%03d:\trdchr\n", pc);
            break;
        case WRCHR:
            printf("%03d:\twrchr\n", pc);
            break;
        case PUSHG:
            printf("%03d:\tpushg\t%d\n", pc, IMMEDIATE(instr));
            break;
        case POPG:
            printf("%03d:\tpopg\t%d\n", pc, IMMEDIATE(instr));
            break;
        case ASF:
            printf("%03d:\tasf\t%d\n", pc, IMMEDIATE(instr));
            break;
        case RSF:
            printf("%03d:\trsf\n", pc);
            break;
        case PUSHL:
            printf("%03d:\tpushl\t%d\n", pc, SIGN_EXTEND(IMMEDIATE(instr)));
            break;
        case POPL:
            printf("%03d:\tpopl\t%d\n", pc, SIGN_EXTEND(IMMEDIATE(instr)));
            break;
        case EQ:
            printf("%03d:\teq\n", pc);
            break;
        case NE:
            printf("%03d:\tne\n", pc);
            break;
        case LT:
            printf("%03d:\tlt\n", pc);
            break;
        case LE:
            printf("%03d:\tle\n", pc);
            break;
        case GT:
            printf("%03d:\tgt\n", pc);
            break;
        case GE:
            printf("%03d:\tge\n", pc);
            break;
        case JMP:
            printf("%03d:\tjmp\t%d\n", pc, IMMEDIATE(instr));
            break;
        case BRF:
            printf("%03d:\tbrf\t%d\n", pc, IMMEDIATE(instr));
            break;
        case BRT:
            printf("%03d:\tbrt\t%d\n", pc, IMMEDIATE(instr));
            break;
        case CALL:
            printf("%03d:\tcall\t%d\n", pc, IMMEDIATE(instr));
            break;
        case RET:
            printf("%03d:\tret\n", pc);
            break;
        case DROP:
            printf("%03d:\tdrop\t%d\n", pc, IMMEDIATE(instr));
            break;
        case PUSHR:
            printf("%03d:\tpushr\n", pc);
            break;
        case POPR:
            printf("%03d:\tpopr\n", pc);
            break;
        case DUP:
            printf("%03d:\tdup\n", pc);
            break;
    }
}

void print_program(void) {
    unsigned int instr;
    int pc = 0;
    do {
        instr = program_memory[pc];
        pc++;
        print_instruction(instr, pc - 1);
    } while (instr >> 24 != HALT);
}


void push_stack(StackSlot s) {
    if(stack_pointer == MAXITEMS) {
        printf("Error: stack overflow\n");
        exit(0);
    }
    stack[stack_pointer] = s;
    stack_pointer++;
}

StackSlot pop_stack() {
    if(stack_pointer == 0) {
        printf("Error: stack underflow\n");
        exit(0);
    }
    stack_pointer--;
    StackSlot tmp = stack[stack_pointer];
    //null stack[stack_pointer]???
    return tmp;
}

StackSlot new_intStackSlot(unsigned int x) {
    ObjRef object;
    StackSlot slot;

    if((object = malloc(sizeof(unsigned int) + sizeof (unsigned int))) == NULL)
        perror("malloc");

    object->size = sizeof (unsigned int);
    *(unsigned int *) object->data = x;

    slot.isObjRef = true;
    slot.u.objRef = object;
    return slot;
}

StackSlot new_charStackSlot(char x) {
    ObjRef object;
    StackSlot slot;

    if((object = malloc(sizeof(unsigned int) + sizeof(char))) == NULL)
        perror("malloc");

    object->size = sizeof(char);
    *(char *) object->data = x;

    slot.isObjRef = true;
    slot.u.objRef = object;
    return slot;
}

unsigned long object_value(long * address) {
    return *((ObjRef) address)->data;
}


void execute_instruction(unsigned int instr) {
    int value;
    char c;
    StackSlot left, right;
    StackSlot slot;
    switch (instr >> 24) {
        case HALT:
            break;
        case PUSHC:
            push_stack(new_intStackSlot(SIGN_EXTEND(instr & 0x00FFFFFF)));
            break;
        case ADD:
            right = pop_stack();
            left = pop_stack();
            if (right.isObjRef && left.isObjRef) {
                if (right.u.objRef->size == sizeof(unsigned int) && left.u.objRef->size == sizeof(unsigned int)) {
                    push_stack(new_intStackSlot(*(unsigned int *) left.u.objRef->data + *(unsigned int *) right.u.objRef->data));
                }
            }
            break;
        case SUB:
            right = pop_stack();
            left = pop_stack();
            if (right.isObjRef && left.isObjRef) {
                if (right.u.objRef->size == sizeof(unsigned int) && left.u.objRef->size == sizeof(unsigned int)) {
                    push_stack(new_intStackSlot(*(unsigned int *) left.u.objRef->data - *(unsigned int *) right.u.objRef->data));
                }
            }
            break;
        case MUL:
            right = pop_stack();
            left = pop_stack();
            if (right.isObjRef && left.isObjRef) {
                if (right.u.objRef->size == sizeof(unsigned int) && left.u.objRef->size == sizeof(unsigned int)) {
                    push_stack(new_intStackSlot(*(unsigned int *) left.u.objRef->data * *(unsigned int *) right.u.objRef->data));
                }
            }
            break;
        case DIV:
            right = pop_stack();
            left = pop_stack();
            if (right.isObjRef && left.isObjRef) {
                if (right.u.objRef->size == sizeof(unsigned int) && left.u.objRef->size == sizeof(unsigned int)) {
                    if(*(unsigned int *) right.u.objRef->data == 0) {
                        printf("Error: Division by zero\n");
                        exit(0);
                    }
                    push_stack(new_intStackSlot(*(unsigned int *) left.u.objRef->data / *(unsigned int *) right.u.objRef->data));
                }
            }
            break;
        case MOD:
            right = pop_stack();
            left = pop_stack();
            if (right.isObjRef && left.isObjRef) {
                if (right.u.objRef->size == sizeof(unsigned int) && left.u.objRef->size == sizeof(unsigned int)) {
                    if(*(unsigned int *) right.u.objRef->data == 0) {
                        printf("Error: Modulo by zero\n");
                        exit(0);
                    }
                    push_stack(new_intStackSlot(*(unsigned int *) left.u.objRef->data % *(unsigned int *) right.u.objRef->data));
                }
            }
            break;
        case RDINT:
            scanf("%d", &value);
            push_stack(new_intStackSlot(value));
            break;
        case WRINT:
            printf("%d", *(unsigned int *) pop_stack().u.objRef->data);
            break;
        case RDCHR:
            scanf("%c", &c);
            push_stack(new_charStackSlot(c));
            break;
        case WRCHR:
            printf("%c", *(char *) pop_stack().u.objRef->data);
            break;
        case PUSHG:
            // Das n-te Element der SDA wird auf dem Stack abgelegt
            push_stack(new_intStackSlot(*(unsigned int *) sda[IMMEDIATE(instr)]->data));
            break;
        case POPG:
            // Der Wert value wird in der SDA als n-tes Element gespeichert
            sda[IMMEDIATE(instr)] = pop_stack().u.objRef;
            break;
        case ASF:
            slot.isObjRef = false;
            slot.u.number = frame_pointer;
            push_stack(slot); // save current fp on stack
            frame_pointer = stack_pointer; // set start of frame
            stack_pointer = stack_pointer + IMMEDIATE(instr); // allocate n vars in frame
            break;
        case RSF:
            stack_pointer = frame_pointer; // points to old fp value
            frame_pointer = pop_stack().u.number;   // set fp to old value
            break;
        case PUSHL:
            push_stack(stack[frame_pointer + SIGN_EXTEND(IMMEDIATE(instr))]);
            break;
        case POPL:
            stack[frame_pointer + SIGN_EXTEND(IMMEDIATE(instr))] = pop_stack();
            break;
        case EQ:
            right = pop_stack();
            left = pop_stack();
            push_stack(new_intStackSlot(*(unsigned int *) left.u.objRef->data == *(unsigned int *) right.u.objRef->data ? 1 : 0));
            break;
        case NE:
            right = pop_stack();
            left = pop_stack();
            push_stack(new_intStackSlot(*(unsigned int *) left.u.objRef->data != *(unsigned int *) right.u.objRef->data ? 1 : 0));
            break;
        case LT:
            right = pop_stack();
            left = pop_stack();
            push_stack(new_intStackSlot(*(unsigned int *) left.u.objRef->data < *(unsigned int *) right.u.objRef->data ? 1 : 0));
            break;
        case LE:
            right = pop_stack();
            left = pop_stack();
            push_stack(new_intStackSlot(*(unsigned int *) left.u.objRef->data <= *(unsigned int *) right.u.objRef->data ? 1 : 0));
            break;
        case GT:
            right = pop_stack();
            left = pop_stack();
            push_stack(new_intStackSlot(*(unsigned int *) left.u.objRef->data > *(unsigned int *) right.u.objRef->data ? 1 : 0));
            break;
        case GE:
            right = pop_stack();
            left = pop_stack();
            push_stack(new_intStackSlot(*(unsigned int *) left.u.objRef->data >= *(unsigned int *) right.u.objRef->data ? 1 : 0));
            break;
        case JMP:
            program_counter = IMMEDIATE(instr);
            break;
        case BRF:
            if(*(unsigned int *)pop_stack().u.objRef->data == 0) program_counter = IMMEDIATE(instr);
            break;
        case BRT:
            if(*(unsigned int *)pop_stack().u.objRef->data == 1) program_counter = IMMEDIATE(instr);
            break;
        case CALL:
            push_stack(new_intStackSlot(program_counter));
            program_counter = IMMEDIATE(instr);
            break;
        case RET:
            program_counter = *(unsigned int *)pop_stack().u.objRef->data;
            break;
        case DROP:
            for(int i = 0; i < IMMEDIATE(instr); i++) {
                pop_stack();
            }
            break;
        case PUSHR:
            push_stack(new_intStackSlot(*(unsigned int *) return_value_register->data));
            break;
        case POPR:
            return_value_register = pop_stack().u.objRef;
            break;
        case DUP:
            left = pop_stack();
            push_stack(left);
            push_stack(left);
            break;
    }
}

void execute_program(bool debug) {
    int instr;
    char e[20];
    do {
        instr = program_memory[program_counter];
        program_counter++;

        if(debug) {
            info:
            print_instruction(instr, program_counter - 1);
            printf("DEBUG: inspect, list, breakpoint, step, run, quit?\n");
            scanf("%s", e);

            switch (e[0]) {
                case 'i':
                    printf("DEBUG [inspect]: stack, data, object?\n");
                    scanf("%s", e);
                    if(e[0] == 's') print_stack();
                    if(e[0] == 'd') print_sda();
                    if(e[0] == 'o') {
                        printf("Object reference:\n");
                        long * address;
                        scanf("%p", &address);
                        printf("%lu\n", object_value(address));
                    }
                    goto info;
                case 'l':
                    print_program();
                    printf("\t\t--- end of code ---\n");
                    goto info;
                case 'b':
                    break;
                case 's':
                    break;
                case 'r':
                    debug = false;
                    break;
                case 'q':
                    goto end_while;
            }
        }

        execute_instruction(instr);
    } while (instr >> 24 != HALT);
    end_while:
    program_counter = 0;
}

void command_line_arguments(int argc, char *argv[]) {
    bool debug = false;
    if(argc >= 2) {
        // COMMAND LINE ARGUMENTS
        if (argv[1][0] == '-') {
            if (strcmp(argv[1], "--help") == 0) {
                printf("usage: ./njvm [option] [option] ...\n");
                printf("  --debug          start virtual machine in debug mode\n");
                printf("  --version        show version and exit\n");
                printf("  --help           show this help and exit\n");
                exit(0);
            } else if (strcmp(argv[1], "--debug") == 0) {
                if(argc == 2) {
                    printf("Error: no code file specified\n");
                    exit(0);
                } else if(argc >= 4) {
                    printf("Error: more than one code file specified\n");
                    exit(0);
                }
                debug = true;
            } else if (strcmp(argv[1], "--version") == 0) {
                printf("Ninja Virtual Machine version 5 (compiled Sep 23 2015, 10:36:52)\n");
                exit(0);
            } else {
                printf("unknown command line argument '%s', try './njvm --help'\n", argv[1]);
                exit(0);
            }
        } else if (argc >= 3){
            printf("Error: more than one code file specified\n");
            exit(0);
        }
    }

    FILE *f = NULL;
    if((f = fopen(argv[debug ? 2 : 1], "r")) == NULL) {
        printf("Error: cannot open code file '%s'", argv[debug ? 2 : 1]);
        exit(0);
    }

    unsigned int format[4];
    fread(format, sizeof (unsigned int), 4, f);
    // CHECK NJBF // CHECK VERSION
    if((format[0] != 1178749518) | (format[1] != 5)) {
        printf("error bin not matching");
        exit(0);
    }

    if(debug) printf("DEBUG: file '%s' loaded (code size = %d, data size = %d)\n", argv[2], format[2], format[3]);

    // ALLOCATE MEMORY // READ NUMBER OF INSTR // READ NUMBER OF SDA
    program_memory = malloc(format[2]*sizeof (unsigned int));
    sda = malloc(format[3] * sizeof (ObjRef));
    sda_size = format[3];
    fread(program_memory, sizeof (unsigned int), format[2], f);
    fclose(f);

    // RUN PROGRAM
    printf("Ninja Virtual Machine started\n");
    execute_program(debug);
    printf("Ninja Virtual Machine stopped\n");

    free(sda);
    free(program_memory);
    exit(0);
}

int main(int argc, char *argv[]) {
    command_line_arguments(argc, argv);
    return 0;
}