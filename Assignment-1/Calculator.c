#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int MAX = 100;

int main() {
    char expression[MAX];
    int values[MAX];
    char ops[MAX];
    int vtop = -1, otop = -1;

    printf("Enter expression: ");
    fgets(expression, MAX, stdin);
    expression[strcspn(expression, "\n")] = 0;

    int i = 0;
    while (expression[i] != '\0') {
        if (expression[i] == ' ') { 
            i++; 
            continue; 
        }
        if (isdigit(expression[i])) {
            int val = 0;
            while (isdigit(expression[i])) {
                val = val * 10 + (expression[i] - '0');
                i++;
            }
            values[++vtop] = val;
        } else if (strchr("+-*/", expression[i])) {
            ops[++otop] = expression[i];
            i++;
        } else {
            printf("Invalid character in expression.\n");
            return 1;
        }
    }

    for (int j = 0; j <= otop; j++) {
        if (ops[j] == '*' || ops[j] == '/') {
            if (ops[j] == '*') values[j] = values[j] * values[j+1];
            else {
                if (values[j+1] == 0) {
                    printf("Division by zero error!\n");
                    return 1;
                }
                values[j] = values[j] / values[j+1];
            }
            for (int k = j+1; k < vtop; k++) {
                values[k] = values[k+1];
            }
            for (int k = j; k < otop; k++) {
                ops[k] = ops[k+1];
            }
            vtop--; 
            otop--; 
            j--;
        }
    }

    int res = values[0];
    for (int j = 0; j <= otop; j++) {
        if (ops[j] == '+') res += values[j+1];
        else if (ops[j] == '-') res -= values[j+1];
    }

    printf("Result: %d\n", res);
    return 0;
}
