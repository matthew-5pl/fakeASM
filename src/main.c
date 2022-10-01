#include "machine.h"

int main(int argc, char** argv) {
    machine* m = init_machine(NULL);
    parse(m, argv[1]);
    return 0;
}