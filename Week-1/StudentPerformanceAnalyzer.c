#include <stdio.h>

typedef struct Student 
{
    int rollNo;
    char name[50];
    int marks[3];
} Student;

void inputStudents(Student *students, int numStudents) 
{
    for(int studentIndex = 0; studentIndex < numStudents; studentIndex++) 
    {
        scanf("%d %s", &students[studentIndex].rollNo, students[studentIndex].name);
        for(int subIndex = 0; subIndex < 3; subIndex++) 
        {
            scanf("%d", &students[studentIndex].marks[subIndex]);
        }
    }
}

int calculateTotal(const Student *students) 
{
    int total = 0;
    for(int subIndex = 0; subIndex < 3; subIndex++) 
    {
        total += students->marks[subIndex];
    }
    return total;
}

float calculateAverage(const Student *students) 
{
    float total = calculateTotal(students);
    return total / 3.0;
}

char computeGrade(float average) 
{
    if (average >= 85) 
    {   
        return 'A';
    } 
    else if (average >= 70) 
    {   
        return 'B';
    } 
    else if (average >= 50) 
    {   
        return 'C';
    } 
    else if (average >= 35) 
    {   
        return 'D';
    } 
    else 
    {   
        return 'F';
    }
}

void displayPerformance(char grade) 
{
    int stars = 0;
    switch(grade) 
    {
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
        default:  
            break;
    }
    printf("Performance: ");
    for(int starIndex = 0; starIndex < stars; starIndex++) 
    {
        printf("*");
    }
    printf("\n");
}

void displayReport(const Student students[], int numStudents) 
{
    int total;
    float average;
    char grade;

    printf("\nPerformance Report:\n");
    
    for(int studentIndex = 0; studentIndex < numStudents; studentIndex++) 
    {
        total = calculateTotal(&students[studentIndex]);
        average = calculateAverage(&students[studentIndex]);
        grade = computeGrade(average);

        printf("\nRoll: %d\nName: %s\nTotal: %d\nAverage: %.2f\nGrade: %c\n", students[studentIndex].rollNo, students[studentIndex].name, total, average, grade);

        if(average < 35) 
        {
            continue;
        }
        displayPerformance(grade);
    }
}

void printRollNumbers(const Student students[], int numStudents, int currentIndex) 
{
    if(currentIndex >= numStudents) 
    {
        return;
    }
    printf("%d ", students[currentIndex].rollNo);
    printRollNumbers(students, numStudents, currentIndex + 1);
}

int main() 
{
    int numStudents;

    printf("Enter number of students: ");
    scanf("%d", &numStudents);

    if(numStudents > 0) 
    {
        Student students[numStudents];

        inputStudents(students, numStudents);

        displayReport(students, numStudents);

        printf("\nList of Roll Numbers (via recursion): ");
        printRollNumbers(students, numStudents, 0);

    } 
    else 
    {
        printf("Wrong Input.\n");
    }

    return 0;
}
