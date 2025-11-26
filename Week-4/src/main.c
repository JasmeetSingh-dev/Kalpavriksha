#include "../inc/commandParser.h"
#include "../inc/directory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct FreeBlock
{
    int index;
    struct FreeBlock *prev, *next;
} FreeBlock;

int NUM_BLOCKS = BLOCK_SIZE;
unsigned char **diskBytes = NULL;

static FreeBlock *freeHead = NULL;
static FreeBlock *freeTail = NULL;
size_t freeCount = 0;

void freelist_push_back(int index)
{
    FreeBlock *n = (FreeBlock*)malloc(sizeof(FreeBlock));
    if (!n) { perror("malloc"); exit(1); }
    n->index = index; n->prev = freeTail; n->next = NULL;
    if (!freeHead)
    {
        freeHead = freeTail = n;
    }
    else
    {
        freeTail->next = n;
        freeTail = n;
    }
    freeCount++;
}

int freelist_pop_front(void)
{
    if (!freeHead) return -1;
    FreeBlock *h = freeHead;
    int idx = h->index;
    freeHead = h->next;
    if (freeHead)
    {
        freeHead->prev = NULL;
    }
    else
    {
        freeTail = NULL;
    }
    free(h);
    freeCount--;
    return idx;
}

void freelist_clear(void)
{
    FreeBlock *p = freeHead;
    while (p)
    {
        FreeBlock *n = p->next;
        free(p);
        p = n;
    }
    freeHead = freeTail = NULL;
    freeCount = 0;
}

void disk_init(int blocks)
{
    NUM_BLOCKS = blocks;
    diskBytes = (unsigned char**)malloc(NUM_BLOCKS * sizeof(unsigned char*));
    if (!diskBytes) { perror("malloc"); exit(1); }
    for (int i = 0; i < NUM_BLOCKS; ++i)
    {
        diskBytes[i] = (unsigned char*)calloc(BLOCK_SIZE, 1);
        if (!diskBytes[i]) { perror("calloc"); exit(1); }
        freelist_push_back(i);
    }
}

void disk_free(void)
{
    if (diskBytes)
    {
        for (int i = 0; i < NUM_BLOCKS; ++i)
        {
            free(diskBytes[i]);
        }
        free(diskBytes);
        diskBytes = NULL;
    }
    freelist_clear();
}

static const char* leaf_name(FileNode *n)
{
    return (n == root) ? "/" : n->name;
}

static void show_prompt(void)
{
    if (cwd == root)
    {
        printf("/ > ");
    }
    else
    {
        printf("%s > ", leaf_name(cwd));
    }
    fflush(stdout);
}

FileNode *root = NULL;
FileNode *cwd  = NULL;

int main(void)
{
    const char *env = getenv("VFS_BLOCKS");
    int blocks = BLOCK_SIZE;
    if (env && *env)
    {
        int n = atoi(env);
        if (n >= 1 && n <= 5000)
        {
            blocks = n;
        }
    }

    disk_init(blocks);
    root = make_node("/", NODE_DIR);
    cwd = root;

    printf("Compact VFS - ready. Type 'exit' to quit.\n");

    char line[4096];
    for (;;)
    {
        show_prompt();
        if (!fgets(line, sizeof(line), stdin))
        {
            break;
        }
        char *s = trim(line);
        if (*s == '\0')
        {
            continue;
        }

        char *cursor = s;
        char *cmd = next_token(&cursor);
        if (!cmd)
        {
            continue;
        }

        if (strcmp(cmd, "ls") == 0)
        {
            cmd_ls();
        }
        else if (strcmp(cmd, "pwd") == 0)
        {
            cmd_pwd();
        }
        else if (strcmp(cmd, "df") == 0)
        {
            cmd_df();
        }
        else if (strcmp(cmd, "mkdir") == 0)
        {
            char *name = next_token(&cursor);
            if (!name) { continue; }
            cmd_mkdir(name);
        }
        else if (strcmp(cmd, "rmdir") == 0)
        {
            char *name = next_token(&cursor);
            if (!name) { continue; }
            cmd_rmdir(name);
        }
        else if (strcmp(cmd, "create") == 0)
        {
            char *name = next_token(&cursor);
            if (!name) { continue; }
            cmd_create(name);
        }
        else if (strcmp(cmd, "write") == 0)
        {
            char *name = next_token(&cursor);
            char *content = next_token(&cursor);
            if (!name) { continue; }
            cmd_write(name, content);
        }
        else if (strcmp(cmd, "read") == 0)
        {
            char *name = next_token(&cursor);
            if (!name) { continue; }
            cmd_read(name);
        }
        else if (strcmp(cmd, "delete") == 0)
        {
            char *name = next_token(&cursor);
            if (!name) { continue; }
            cmd_delete(name);
        }
        else if (strcmp(cmd, "cd") == 0)
        {
            char *name = next_token(&cursor);
            if (!name) { continue; }
            cmd_cd(name);
        }
        else if (strcmp(cmd, "exit") == 0)
        {
            cmd_exit();
            break;
        }
        else
        {
            printf("Unknown command: %s\n", cmd);
        }
    }

    return 0;
}