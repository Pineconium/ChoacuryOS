#include "calc.h"

#include "../../terminal.h"
#include "../../../drivers/vga.h"
#include "../../../drivers/vbe.h"
#include "../../../gui/desktop.h"

typedef int (*math_op_t)(int, int);
int add(int a, int b);
int subtract(int a, int b);
int divide(int a, int b);
int multiply(int a, int b);
typedef struct {
    const char *op;
    math_op_t func;
} op_map_t;

op_map_t operations[] = {
    {"+", add},
    {"-a", add},
    {"-", subtract},
    {"-s", subtract},
    {"/", divide},
    {"-d", divide},
    {"*", multiply},
    {"-m", multiply},
    {NULL, NULL} // <- End of map
};
int add(int a, int b) {
    return a + b;
}

int subtract(int a, int b) {
    return a - b;
}

int divide(int a, int b) {
    return a / b;
}

int multiply(int a, int b) {
    return a * b;
}

int shell_calc_command(int argc, const char** argv) {
    if (argc != 4) {
        return 2;
    }

    atoi_result_t number1 = atoi(argv[1]);
    atoi_result_t number2 = atoi(argv[3]);
    if (!number1.valid || !number2.valid) {
        return 2;
    }

    math_op_t op_func = NULL;
    for (int i = 0; operations[i].op != NULL; i++) {
        if (strcmp(argv[2], operations[i].op) == 0) {
            op_func = operations[i].func;
            break;
        }
    }

    if (op_func == NULL) {
        return 2;
    }

    // Perform the calculation and handle division by zero
    int result = op_func(number1.value, number2.value);
    if (op_func == divide && number2.value == 0) {
        term_write("ERROR: Cannot divide by 0!\n", TC_LRED);
    } else {
        term_write_u32(result, 10, TC_WHITE);
        term_write("\n", TC_WHITE);
    }

    return 0;
}