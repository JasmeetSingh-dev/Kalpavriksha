#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX 100

int parseExpression(const char *expression, int numbers[], char operators[], int *numTop, int *opTop) 
{
    int index = 0;
    while (expression[index] != '\0') 
    {
        if (isspace(expression[index])) 
        {
            index++;
            continue;
        }

        if (isdigit(expression[index])) 
        {
            int numberValue = 0;
            while (isdigit(expression[index])) 
            {
                numberValue = numberValue * 10 + (expression[index] - '0');
                index++;
            }
            numbers[++(*numTop)] = numberValue;
        } 
        else if (strchr("+-*/", expression[index])) 
        {
            operators[++(*opTop)] = expression[index];
            index++;
        } 
        else 
        {
            printf("Invalid character '%c' in expression.\n", expression[index]);
            return 0;
        }
    }
    return 1;
}

void evaluateMultiplicationDivision(int numbers[], char operators[], int *numTop, int *opTop) 
{
    int operatorIndex = 0;
    while (operatorIndex <= *opTop) 
    {
        if (operators[operatorIndex] == '*' || operators[operatorIndex] == '/') 
        {
            if (operators[operatorIndex] == '*') 
            {
                numbers[operatorIndex] = numbers[operatorIndex] * numbers[operatorIndex + 1];
            } 
            else 
            {
                if (numbers[operatorIndex + 1] == 0) 
                {
                    printf("Division by zero error!\n");
                    exit(1);
                }
                numbers[operatorIndex] = numbers[operatorIndex] / numbers[operatorIndex + 1];
            }

            for (int shiftIndexNumbers = operatorIndex + 1; shiftIndexNumbers < *numTop; shiftIndexNumbers++) 
            {
                numbers[shiftIndexNumbers] = numbers[shiftIndexNumbers + 1];
            }

            for (int shiftIndexOperators = operatorIndex; shiftIndexOperators < *opTop; shiftIndexOperators++) 
            {
                operators[shiftIndexOperators] = operators[shiftIndexOperators + 1];
            }

            (*numTop)--;
            (*opTop)--;
            operatorIndex--;
        }
        operatorIndex++;
    }
}

int evaluateAdditionSubtraction(int numbers[], char operators[], int numTop, int opTop) 
{
    int resultValue = numbers[0];
    int operatorIndex = 0;

    while (operatorIndex <= opTop) 
    {
        if (operators[operatorIndex] == '+') 
        {
            resultValue += numbers[operatorIndex + 1];
        } 
        else if (operators[operatorIndex] == '-') 
        {
            resultValue -= numbers[operatorIndex + 1];
        }
        operatorIndex++;
    }
    return resultValue;
}

int main() 
{
    char expression[MAX];
    int numbers[MAX];
    char operators[MAX];
    int numTop = -1;
    int opTop = -1;

    printf("Enter expression: ");
    fgets(expression, MAX, stdin);
    expression[strcspn(expression, "\n")] = 0;

    if (!parseExpression(expression, numbers, operators, &numTop, &opTop)) 
    {
        return 1;
    }

    evaluateMultiplicationDivision(numbers, operators, &numTop, &opTop);

    int finalResult = evaluateAdditionSubtraction(numbers, operators, numTop, opTop);

    printf("Result: %d\n", finalResult);
    return 0;
}
