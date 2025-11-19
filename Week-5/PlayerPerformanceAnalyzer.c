#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Players_data.h"

#define MAX_NAME 50
#define MAX_TEAMS 10

typedef enum 
{ 
    BATSMAN = 1,
    BOWLER = 2, 
    ALLROUNDER = 3 
} Role;

typedef struct PlayerNode 
{
    int id;
    char name[MAX_NAME];
    char team[MAX_NAME];
    Role role;
    int totalRuns;
    float battingAverage;
    float strikeRate;
    int wickets;
    float economyRate;
    float performanceIndex;
    struct PlayerNode *next;
} PlayerNode;

typedef struct Team
{
    int teamId;
    char name[MAX_NAME];
    int totalPlayers;
    float avgBatStrikeRate;
    PlayerNode *playerList;
} Team;

Team teamsList[MAX_TEAMS];

int getRole(const char *role) 
{
    if (strcmp(role, "Batsman") == 0) 
    {
        return BATSMAN;
    }
    if (strcmp(role, "Bowler") == 0) 
    {
        return BOWLER;
    }
    return ALLROUNDER;
}

void computePerformance(PlayerNode *p) 
{
    if (p->role == BATSMAN)
    {
        p->performanceIndex = (p->battingAverage * p->strikeRate) / 100.0;
    }
    else if (p->role == BOWLER)
    {
        p->performanceIndex = (p->wickets * 2) + (100 - p->economyRate);
    }
    else
    {
        p->performanceIndex = (p->battingAverage * p->strikeRate) / 100.0 + (p->wickets * 2);
    }
}

void initializeTeams() 
{
    for (int i = 0; i < teamCount; i++) 
    {
        teamsList[i].teamId = i + 1;
        strcpy(teamsList[i].name, teams[i]);
        teamsList[i].playerList = NULL;
        teamsList[i].totalPlayers = 0;
        teamsList[i].avgBatStrikeRate = 0;
    }
}

int binarySearchTeam(const char *teamName) 
{
    int l = 0, r = teamCount - 1;
    while (l <= r) 
    {
        int mid = (l + r) / 2;
        int cmp = strcmp(teamsList[mid].name, teamName);
        if (cmp == 0) return mid;
        if (cmp < 0) l = mid + 1;
        else r = mid - 1;
    }
    return -1;
}

PlayerNode *createPlayer(const Player *src) 
{
    PlayerNode *p = malloc(sizeof(PlayerNode));
    p->id = src->id;
    strcpy(p->name, src->name);
    strcpy(p->team, src->team);
    p->role = getRole(src->role);
    p->totalRuns = src->totalRuns;
    p->battingAverage = src->battingAverage;
    p->strikeRate = src->strikeRate;
    p->wickets = src->wickets;
    p->economyRate = src->economyRate;
    computePerformance(p);
    p->next = NULL;
    return p;
}

void initializePlayers() 
{
    for (int i = 0; i < playerCount; i++) 
    {
        int idx = binarySearchTeam(players[i].team);
        if (idx == -1) 
        {
            continue;
        }
        PlayerNode *p = createPlayer(&players[i]);
        p->next = teamsList[idx].playerList;
        teamsList[idx].playerList = p;
        teamsList[idx].totalPlayers++;
    }
}

void addPlayerToTeam(int teamId) 
{
    int idx = teamId - 1;
    PlayerNode *p = malloc(sizeof(PlayerNode));

    printf("Player ID: ");
    scanf("%d", &p->id);
    printf("Name: ");
    getchar();
    fgets(p->name, MAX_NAME, stdin);
    p->name[strcspn(p->name, "\n")] = 0;

    printf("Role (1-Batsman,2-Bowler,3-All-rounder): ");
    int r; 
    scanf("%d", &r);
    p->role = r;

    printf("Total Runs: "); 
    scanf("%d", &p->totalRuns);
    printf("Batting Avg: "); 
    scanf("%f", &p->battingAverage);
    printf("Strike Rate: "); 
    scanf("%f", &p->strikeRate);
    printf("Wickets: "); 
    scanf("%d", &p->wickets);
    printf("Economy Rate: "); 
    scanf("%f", &p->economyRate);

    strcpy(p->team, teamsList[idx].name);

    computePerformance(p);

    p->next = teamsList[idx].playerList;
    teamsList[idx].playerList = p;
    teamsList[idx].totalPlayers++;

    printf("\nPlayer added successfully to %s!\n", teamsList[idx].name);
}

void computeTeamStats(int idx) 
{
    PlayerNode *p = teamsList[idx].playerList;
    float sum = 0;
    int cnt = 0;
    while (p) 
    {
        if (p->role == BATSMAN || p->role == ALLROUNDER) 
        {
            sum += p->strikeRate;
            cnt++;
        }
        p = p->next;
    }
    teamsList[idx].avgBatStrikeRate = (cnt == 0 ? 0 : sum / cnt);
}

void displayTeamPlayers() {
    int id;
    printf("Enter Team ID: ");
    scanf("%d", &id);
    int idx = id - 1;
    computeTeamStats(idx);

    PlayerNode *p = teamsList[idx].playerList;

    printf("\nPlayers of %s\n", teamsList[idx].name);
    printf("===============================================================\n");
    printf("ID   Name               Role Runs Avg SR Wkts ER Perf\n");
    printf("===============================================================\n");

    while (p) {
        printf("%d  %-18s %d   %d  %.1f %.1f %d %.1f %.2f\n",
            p->id, p->name, p->role, p->totalRuns,
            p->battingAverage, p->strikeRate,
            p->wickets, p->economyRate, p->performanceIndex
        );
        p = p->next;
    }

    printf("\nTotal Players: %d\n", teamsList[idx].totalPlayers);
    printf("Avg Strike Rate: %.2f\n", teamsList[idx].avgBatStrikeRate);
}

void sortTeamsByStrikeRate() 
{
    for (int i = 0; i < teamCount; i++)
    {
        computeTeamStats(i);
    }

    for (int i = 0; i < teamCount; i++) 
    {
        for (int j = i + 1; j < teamCount; j++) 
        {
            if (teamsList[j].avgBatStrikeRate > teamsList[i].avgBatStrikeRate) 
            {
                Team temp = teamsList[i];
                teamsList[i] = teamsList[j];
                teamsList[j] = temp;
            }
        }
    }
}

void displayTeamsSorted() 
{
    sortTeamsByStrikeRate();

    printf("\n=======================================================\n");
    printf("ID  Team Name       AvgSR   Players\n");
    printf("=======================================================\n");

    for (int i = 0; i < teamCount; i++) 
    {
        printf("%d  %-15s %.2f   %d\n",
            teamsList[i].teamId, teamsList[i].name,
            teamsList[i].avgBatStrikeRate, teamsList[i].totalPlayers
        );
    }
}

void displayTopK() 
{
    int teamId, roleInput, K;
    printf("Enter Team ID: ");
    scanf("%d", &teamId);
    printf("Enter Role (1-Batsman,2-Bowler,3-All-rounder): ");
    scanf("%d", &roleInput);
    printf("Enter K: ");
    scanf("%d", &K);

    int idx = teamId - 1;

    PlayerNode *list = NULL;
    PlayerNode *p = teamsList[idx].playerList;

    while (p) 
    {
        if (p->role == roleInput) 
        {
            PlayerNode *copy = malloc(sizeof(PlayerNode));
            *copy = *p;
            copy->next = list;
            list = copy;
        }
        p = p->next;
    }

    PlayerNode *a = list;
    PlayerNode *b = list;

    while (a) 
    {
        b = a->next;
        while (b) 
        {
            if (b->performanceIndex > a->performanceIndex) 
            {
                PlayerNode temp = *a;
                *a = *b;
                *b = temp;
            }
            b = b->next;
        }
        a = a->next;
    }

    printf("\nTop %d Players:\n", K);
    int c = 0;
    p = list;
    while (p && c < K) 
    {
        printf("%d %-18s PI=%.2f\n", p->id, p->name, p->performanceIndex);
        p = p->next;
        c++;
    }
}

void displayGlobalRoleWise() 
{
    int r;
    printf("Enter Role (1-B,2-B,3-AR): ");
    scanf("%d", &r);

    PlayerNode *list = NULL;

    for (int i = 0; i < teamCount; i++) 
    {
        PlayerNode *p = teamsList[i].playerList;
        while (p) 
        {
            if (p->role == r) 
            {
                PlayerNode *copy = malloc(sizeof(PlayerNode));
                *copy = *p;
                copy->next = list;
                list = copy;
            }
            p = p->next;
        }
    }

    PlayerNode *a = list;
    PlayerNode *b = list;

    while (a) 
    {
        b = a->next;
        while (b) 
        {
            if (b->performanceIndex > a->performanceIndex) 
            {
                PlayerNode temp = *a;
                *a = *b;
                *b = temp;
            }
            b = b->next;
        }
        a = a->next;
    }

    printf("\nRole-wise sorted list:\n");

    PlayerNode *p = list;
    while (p) 
    {
        printf("%d %-18s %-12s PI=%.2f\n",
            p->id, p->name, p->team, p->performanceIndex
        );
        p = p->next;
    }
}

void runMenu() 
{
    int choice;

    while (1) 
    {
        printf("\n========================================\n");
        printf(" ICC ODI Player Performance Analyzer\n");
        printf("========================================\n");
        printf("1. Add Player to Team\n");
        printf("2. Display Players of a Team\n");
        printf("3. Display Teams by Avg Strike Rate\n");
        printf("4. Display Top K Players of a Team\n");
        printf("5. Display Players Across All Teams (Role-wise)\n");
        printf("6. Exit\n");
        printf("Enter Choice: ");

        scanf("%d", &choice);

        switch (choice) 
        {
            case 1: 
            {
                int id;
                printf("Enter Team ID: ");
                scanf("%d", &id);
                addPlayerToTeam(id);
                break;
            }
            case 2:
                displayTeamPlayers();
                break;
            case 3:
                displayTeamsSorted();
                break;
            case 4:
                displayTopK();
                break;
            case 5:
                displayGlobalRoleWise();
                break;
            case 6:
                exit(0);
            default:
                printf("Invalid choice!\n");
        }
    }
}

int main() 
{
    initializeTeams();
    initializePlayers();
    runMenu();
    return 0;
}
