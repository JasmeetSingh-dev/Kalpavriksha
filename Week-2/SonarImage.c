#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void displayMatrix(const int *matrix, const int matrixSize)
{
    for (int row = 0; row < matrixSize; row++)
    {
        for (int col = 0; col < matrixSize; col++)
        {
            printf("%3d ", *(matrix + row * matrixSize + col));
        }
        printf("\n");
    }
    printf("\n");
}

void initializeMatrix(int *matrix, const int matrixSize)
{
    for (int index = 0; index < matrixSize * matrixSize; index++)
    {
        *(matrix + index) = rand() % 256;
    }
}

void freeMatrix(int *matrix)
{
    free(matrix);
}

void rotateClockwise(int *matrix, const int matrixSize)
{
    int tempValue;

    for (int row = 0; row < matrixSize; row++)
    {
        for (int col = row + 1; col < matrixSize; col++)
        {
            int *firstElement = matrix + row * matrixSize + col;
            int *secondElement = matrix + col * matrixSize + row;
            tempValue = *firstElement;
            *firstElement = *secondElement;
            *secondElement = tempValue;
        }
    }

    for (int row = 0; row < matrixSize; row++)
    {
        for (int col = 0; col < matrixSize / 2; col++)
        {
            int *leftElement = matrix + row * matrixSize + col;
            int *rightElement = matrix + row * matrixSize + (matrixSize - col - 1);
            tempValue = *leftElement;
            *leftElement = *rightElement;
            *rightElement = tempValue;
        }
    }
}

void applySmoothing(int *matrix, const int matrixSize)
{
    int *temporaryRow = (int *)malloc(matrixSize * sizeof(int));

    for (int row = 0; row < matrixSize; row++)
    {
        for (int col = 0; col < matrixSize; col++)
        {
            int sumOfNeighbors = 0;
            int validNeighborCount = 0;

            for (int rowOffset = -1; rowOffset <= 1; rowOffset++)
            {
                for (int colOffset = -1; colOffset <= 1; colOffset++)
                {
                    int neighborRow = row + rowOffset;
                    int neighborCol = col + colOffset;

                    if (neighborRow >= 0 && neighborRow < matrixSize &&
                        neighborCol >= 0 && neighborCol < matrixSize)
                    {
                        sumOfNeighbors += *(matrix + neighborRow * matrixSize + neighborCol);
                        validNeighborCount++;
                    }
                }
            }
            *(temporaryRow + col) = sumOfNeighbors / validNeighborCount;
        }

        for (int col = 0; col < matrixSize; col++)
        {
            *(matrix + row * matrixSize + col) = *(temporaryRow + col);
        }
    }

    free(temporaryRow);
}

int main()
{
    int matrixSize;
    printf("Enter matrix size (2 - 10): ");
    scanf("%d", &matrixSize);

    if (matrixSize < 2 || matrixSize > 10)
    {
        printf("Invalid matrix size.\n");
        return -1;
    }

    srand(time(NULL));

    int *matrix = (int *)malloc(matrixSize * matrixSize * sizeof(int));
    if (!matrix)
    {
        printf("Memory allocation failed.\n");
        return -1;
    }

    initializeMatrix(matrix, matrixSize);
    displayMatrix(matrix, matrixSize);
    rotateClockwise(matrix, matrixSize);
    displayMatrix(matrix, matrixSize);
    applySmoothing(matrix, matrixSize);
    displayMatrix(matrix, matrixSize);
    freeMatrix(matrix);

    return 0;
}
