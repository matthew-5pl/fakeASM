#include "machine.h"

int main() {
    machine* m = init_machine(NULL);
    parse(m, "src/test.asm");
    return 0;
}