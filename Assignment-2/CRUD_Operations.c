#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME "users.txt"

struct User 
{
    int id;
    char name[50];
    int age;
};

typedef enum 
{
    ADD_USER = 1,
    SHOW_USERS,
    UPDATE_USER,
    DELETE_USER,
    EXIT
} MenuOption;

void createRecord() 
{
    FILE *file = fopen(FILENAME, "a");
    struct User u;
    if (file == NULL) 
    {
        printf("Cannot open file!\n");
        return;
    }
    printf("Enter ID: ");
    scanf("%d", &u.id);
    printf("Enter Name: ");
    scanf("%s", u.name);
    printf("Enter Age: ");
    scanf("%d", &u.age);

    fprintf(file, "%d %s %d\n", u.id, u.name, u.age);
    fclose(file);
    printf("Record added.\n");
}

void readRecords() 
{
    FILE *file = fopen(FILENAME, "r");
    struct User u;
    if (file == NULL) 
    {
        printf("No records found.\n");
        return;
    }

    printf("\nID\tName\tAge\n");
    while (fscanf(file, "%d %s %d", &u.id, u.name, &u.age) != EOF) 
    {
        printf("%d\t%s\t%d\n", u.id, u.name, u.age);
    }
    fclose(file);
}

void updateRecord() 
{
    FILE *file = fopen(FILENAME, "r");
    FILE *temp = fopen("temp.txt", "w");
    struct User u;
    int id, found = 0;

    if (file == NULL || temp == NULL) 
    {
        printf("File error!\n");
        return;
    }

    printf("Enter ID to update: ");
    scanf("%d", &id);

    while (fscanf(file, "%d %s %d", &u.id, u.name, &u.age) != EOF) 
    {
        if (u.id == id) 
        {
            printf("Enter new Name: ");
            scanf("%s", u.name);
            printf("Enter new Age: ");
            scanf("%d", &u.age);
            found = 1;
        }
        fprintf(temp, "%d %s %d\n", u.id, u.name, u.age);
    }

    fclose(file);
    fclose(temp);

    remove(FILENAME);
    rename("temp.txt", FILENAME);

    if (found)
        printf("Record updated.\n");
    else
        printf("Record not found.\n");
}

void deleteRecord() 
{
    FILE *file = fopen(FILENAME, "r");
    FILE *temp = fopen("temp.txt", "w");
    struct User u;
    int id, found = 0;

    if (file == NULL || temp == NULL) 
    {
        printf("File error!\n");
        return;
    }

    printf("Enter ID to delete: ");
    scanf("%d", &id);

    while (fscanf(file, "%d %s %d", &u.id, u.name, &u.age) != EOF) 
    {
        if (u.id != id) 
        {
            fprintf(temp, "%d %s %d\n", u.id, u.name, u.age);
        } else 
        {
            found = 1;
        }
    }

    fclose(file);
    fclose(temp);

    remove(FILENAME);
    rename("temp.txt", FILENAME);

    if (found)
        printf("Record deleted.\n");
    else
        printf("Record not found.\n");
}

int main() 
{
    int choice;
    while (1) 
    {
        printf("\n1. Add User\n2. Show Users\n3. Update User\n4. Delete User\n5. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch((MenuOption)choice) 
        {
            case ADD_USER:
                createRecord();
                break;
            case SHOW_USERS:
                readRecords();
                break;
            case UPDATE_USER:
                updateRecord();
                break;
            case DELETE_USER:
                deleteRecord();
                break;
            case EXIT:
                exit(0);
            default:
                printf("Invalid choice!\n");
        }
    }
    return 0;
}
