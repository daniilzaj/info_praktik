#include <stdio.h>
#include <limits.h>

int cpu_state[8];
int memory[256];

/* Здесь команды и числа лежат в одном массиве memory. Процессор берёт
   команду по адресу из cpu_state[0], выполняет её и сохраняет результат
   в cpu_state[1]. Так работает модель архитектуры фон Неймана. */

int fault(int code, int pc, int op, int arg) {
    const char *msg[] = {"", "адрес вне памяти", "неизвестная команда", "переполнение"};
    printf("pc=%d: команда %d, operand=%d, ОШИБКА: %s\n", pc, op, arg, msg[code]);
    cpu_state[5] = code;
    if (cpu_state[6] || cpu_state[3] < 0 || cpu_state[3] >= 256) {
        cpu_state[4] = 1;
        return 0;
    }
    cpu_state[6] = 1;
    cpu_state[0] = cpu_state[3];
    return 1;
}

int main(void) {
    int n = 0, extra;
    while (n < 256 && scanf("%d", &memory[n]) == 1) n++;
    if (scanf("%d", &extra) == 1) {
        printf("Программа длиннее 256 ячеек\n");
        return 1;
    }
    if (n < 2 || memory[0] < 0 || memory[0] >= 256 || memory[1] < 2 || memory[1] >= n) {
        printf("Неверный заголовок программы\n");
        return 1;
    }
    cpu_state[0] = memory[1];
    cpu_state[3] = memory[0];
    while (!cpu_state[4]) {
        int pc = cpu_state[0], op, arg = -1;
        if (pc < 0 || pc >= n) {
            if (!fault(1, pc, -1, -1)) break;
            continue;
        }
        op = memory[pc];
        cpu_state[2] = op;
        if (op == 0) {
            cpu_state[4] = 1;
        } else if (op < 1 || op > 5) {
            if (!fault(2, pc, op, -1)) break;
            continue;
        } else if (op == 1) {
            printf("Значение аккумулятора: %d\n", cpu_state[1]);
            cpu_state[0] = pc + 1;
        } else {
            if (pc + 1 >= n) {
                if (!fault(1, pc, op, -1)) break;
                continue;
            }
            arg = memory[pc + 1];
            if (arg < 0 || arg >= n) {
                if (!fault(1, pc, op, arg)) break;
                continue;
            }
            if (op == 2) cpu_state[1] = memory[arg];
            if (op == 3) {
                long long x = (long long)cpu_state[1] * memory[arg];
                if (x < INT_MIN || x > INT_MAX) {
                    if (!fault(3, pc, op, arg)) break;
                    continue;
                }
                cpu_state[1] = (int)x;
            }
            cpu_state[0] = (op == 4 || (op == 5 && cpu_state[1] == 0)) ? arg : pc + 2;
        }
        printf("pc=%d: команда %d, operand=%d, acc=%d\n", pc, op, arg, cpu_state[1]);
    }
    printf("Программа завершена%s.\n", cpu_state[5] ? " с ошибкой" : "");
    return cpu_state[5] ? 1 : 0;
}
