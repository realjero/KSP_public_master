#include <stdio.h>
#include <string.h>

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
    printf("Ninja Virtual Machine stopped\n");
    return 0;
}