#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void displayMatrix(const int *matrix, const int matrixSize)
{
    for (int rowIndex = 0; rowIndex < matrixSize; rowIndex++)
    {
        for (int colIndex = 0; colIndex < matrixSize; colIndex++)
        {
            printf("%3d ", *(matrix + rowIndex * matrixSize + colIndex));
        }
        printf("\n");
    }
    printf("\n");
}

void initializeMatrix(int *matrix, const int matrixSize)
{
    for (int elementIndex = 0; elementIndex < matrixSize * matrixSize; elementIndex++)
    {
        *(matrix + elementIndex) = rand() % 256;
    }
}

void freeMatrix(int *matrix)
{
    free(matrix);
}

void rotateMatrix90(int *matrix, const int matrixSize)
{
    int tempValue;

    for (int rowIndex = 0; rowIndex < matrixSize; rowIndex++)
    {
        for (int colIndex = rowIndex + 1; colIndex < matrixSize; colIndex++)
        {
            int *firstElement = matrix + rowIndex * matrixSize + colIndex;
            int *secondElement = matrix + colIndex * matrixSize + rowIndex;
            tempValue = *firstElement;
            *firstElement = *secondElement;
            *secondElement = tempValue;
        }
    }

    for (int rowIndex = 0; rowIndex < matrixSize; rowIndex++)
    {
        for (int colIndex = 0; colIndex < matrixSize / 2; colIndex++)
        {
            int *leftElement = matrix + rowIndex * matrixSize + colIndex;
            int *rightElement = matrix + rowIndex * matrixSize + (matrixSize - colIndex - 1);
            tempValue = *leftElement;
            *leftElement = *rightElement;
            *rightElement = tempValue;
        }
    }
}

void applySmoothing(int *matrix, const int matrixSize)
{
    int *temporaryRow = (int *)malloc(matrixSize * sizeof(int));

    for (int rowIndex = 0; rowIndex < matrixSize; rowIndex++)
    {
        for (int colIndex = 0; colIndex < matrixSize; colIndex++)
        {
            int sumOfNeighbors = 0;
            int validNeighborCount = 0;

            for (int rowOffset = -1; rowOffset <= 1; rowOffset++)
            {
                for (int colOffset = -1; colOffset <= 1; colOffset++)
                {
                    int neighborRow = rowIndex + rowOffset;
                    int neighborCol = colIndex + colOffset;

                    if (neighborRow >= 0 && neighborRow < matrixSize &&
                        neighborCol >= 0 && neighborCol < matrixSize)
                    {
                        sumOfNeighbors += *(matrix + neighborRow * matrixSize + neighborCol);
                        validNeighborCount++;
                    }
                }
            }
            *(temporaryRow + colIndex) = sumOfNeighbors / validNeighborCount;
        }

        for (int colIndex = 0; colIndex < matrixSize; colIndex++)
        {
            *(matrix + rowIndex * matrixSize + colIndex) = *(temporaryRow + colIndex);
        }
    }

    free(temporaryRow);
}

int main()
{
    int matrixSize;
    printf("Enter matrix size (2 – 10): ");
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
    rotateMatrix90(matrix, matrixSize);
    displayMatrix(matrix, matrixSize);
    applySmoothing(matrix, matrixSize);
    displayMatrix(matrix, matrixSize);
    freeMatrix(matrix);

    return 0;
}
