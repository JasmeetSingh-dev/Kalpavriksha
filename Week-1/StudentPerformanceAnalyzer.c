#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Student {
    int rollNo;
    char name[50];
    int marks[3];
};

int calculateTotal(struct Student *s) {
    int total = 0;
    for(int i = 0; i < 3; i++) {
        total += s->marks[i];
    }
    return total;
}

float calculateAverage(struct Student *s) {
    float total = calculateTotal(s);
    return total / 3.0;
}

char assignGrade(float average) {
    if (average >= 85) return 'A';
    else if (average >= 70) return 'B';
    else if (average >= 50) return 'C';
    else if (average >= 35) return 'D';
    else return 'F';
}

void displayPerformance(char grade) {
    int stars = 0;
    switch(grade) {
        case 'A': 
            stars = 5; 
            break;
        case 'B': 
            stars = 4; 
            break;
        case 'C': 
            stars = 3; 
            break;
        case 'D': 
            stars = 2; 
            break;
        case 'F':  
            break;
    }
    printf("Performance: ");
    for(int i = 0; i < stars; i++) {
        printf("*");
    }
    printf("\n");
}

void sortRollNumbers(int rollNumbers[], int numStudents) {
    for(int i = 0; i < numStudents - 1; i++) {
        for(int j = 0; j < numStudents - i - 1; j++) {
            if(rollNumbers[j] > rollNumbers[j + 1]) {
                int temp = rollNumbers[j];
                rollNumbers[j] = rollNumbers[j + 1];
                rollNumbers[j + 1] = temp;
            }
        }
    }
}

void printRollNumbers(int rollNumbers[], int numStudents, int index) {
    if(index >= numStudents) {
        return;
    }
    printf("%d ", rollNumbers[index]);
    printRollNumbers(rollNumbers, numStudents, index + 1);
}

int main() {
    int numStudents;
    int total;
    float average;
    char grade;

    printf("Input:\n\n");
    scanf("%d", &numStudents);

    if(numStudents <= 0) {
        printf("Wrong Input.\n");
        return 0;
    }

    struct Student students[numStudents];
    int rollNumbers[numStudents];

    for(int i = 0; i < numStudents; i++) {
        scanf("%d %s", &students[i].rollNo, students[i].name);
        for(int j = 0; j < 3; j++) {
            scanf("%d", &students[i].marks[j]);
        }
    }

    printf("\nOutput:\n");

    for(int i = 0; i < numStudents; i++) {
        total = calculateTotal(&students[i]);
        average = calculateAverage(&students[i]);
        grade = assignGrade(average);

        rollNumbers[i] = students[i].rollNo;

        printf("\nRoll: %d\nName: %s\nTotal: %d\nAverage: %.2f\nGrade: %c\n", students[i].rollNo, students[i].name, total, average, grade);

        if(average < 35) {
            continue;
        }
        displayPerformance(grade);
    }

    sortRollNumbers(rollNumbers, numStudents);
    printf("\nList of Roll Numbers (via recursion): ");
    printRollNumbers(rollNumbers, numStudents, 0);

    return 0;
}