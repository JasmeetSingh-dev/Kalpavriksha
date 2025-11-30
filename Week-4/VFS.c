#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>

#define BLOCK_SIZE 512
#ifndef DEFAULT_NUM_BLOCKS
#define DEFAULT_NUM_BLOCKS 1024
#endif

#define NAME_MAX_LEN 50

struct FileNode;

typedef struct FreeBlock 
{
    int index;
    struct FreeBlock *prev, *next; 
} FreeBlock;

static FreeBlock *freeHead = NULL; 
static FreeBlock *freeTail = NULL;
static size_t freeCount = 0;       

static void freelist_push_back(int index) 
{
    FreeBlock *n = (FreeBlock*)malloc(sizeof(FreeBlock));
    if (!n) 
    { 
        perror("malloc"); 
        exit(1); 
    }
    n->index = index; n->prev = freeTail; 
    n->next = NULL;
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

static int freelist_pop_front(void) 
{
    if (!freeHead) 
    {
        return -1;
    }
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

static void freelist_clear(void) 
{
    FreeBlock *p = freeHead;
    while (p) 
    { 
        FreeBlock *n = p->next; 
        free(p); 
        p = n; 
    }
    freeHead = freeTail = NULL; freeCount = 0;
}

static int NUM_BLOCKS = DEFAULT_NUM_BLOCKS; 
static unsigned char **diskBytes = NULL; 

static void disk_init(int blocks) 
{
    NUM_BLOCKS = blocks;
    diskBytes = (unsigned char**)malloc(NUM_BLOCKS * sizeof(unsigned char*));
    if (!diskBytes) 
    { 
        perror("malloc"); 
        exit(1); 
    }
    for (int i = 0; i < NUM_BLOCKS; ++i) 
    {
        diskBytes[i] = (unsigned char*)calloc(BLOCK_SIZE, 1);
        if (!diskBytes[i]) 
        { 
            perror("calloc"); 
            exit(1); 
        }
        freelist_push_back(i);
    }
}

static void disk_free(void) 
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

typedef enum 
{ 
    NODE_DIR = 1,
    NODE_FILE = 2 
} NodeType;

typedef struct FileData 
{
    size_t sizeBytes;    
    int   *blocks;         
    int    blockCount;     
    int    blockCap;      
} FileData;

typedef struct FileNode 
{
    char name[NAME_MAX_LEN + 1];
    NodeType type;
    struct FileNode *childHead; 
    struct FileNode *next, *prev;
    struct FileNode *parent;
    FileData file;
} FileNode;

static FileNode *root = NULL;
static FileNode *cwd  = NULL;

static FileNode* make_node(const char *name, NodeType type) 
{
    FileNode *n = (FileNode*)calloc(1, sizeof(FileNode));
    if (!n) 
    { 
        perror("calloc"); 
        exit(1); 
    }
    strncpy(n->name, name, NAME_MAX_LEN);
    n->name[NAME_MAX_LEN] = '\0';
    n->type = type;
    n->childHead = NULL;
    n->next = n->prev = NULL;
    n->parent = NULL;
    if (type == NODE_FILE) 
    {
        n->file.sizeBytes = 0;
        n->file.blocks = NULL;
        n->file.blockCount = 0;
        n->file.blockCap = 0;
    }
    return n;
}

static void free_file_data(FileNode *f) 
{
    if (!f || f->type != NODE_FILE) 
    {
        return;
    }
    free(f->file.blocks);
    f->file.blocks = NULL;
    f->file.blockCap = f->file.blockCount = 0;
    f->file.sizeBytes = 0;
}

static void destroy_node_recursive(FileNode *n) 
{
    if (!n) 
    {
        return;
    }
    if (n->type == NODE_DIR) 
    { 
        FileNode *h = n->childHead;
        if (h) 
        {
            FileNode *p = h;
            do 
            {
                FileNode *nx = p->next;
                destroy_node_recursive(p);
                p = nx;
            } while (p && p != h);
        }
    } 
    else if (n->type == NODE_FILE) 
    {
        for (int i = 0; i < n->file.blockCount; ++i) 
        {
            freelist_push_back(n->file.blocks[i]);
        }
        free_file_data(n);
    }
    free(n);
}

static FileNode* find_child(FileNode *dir, const char *name) 
{
    if (!dir || dir->type != NODE_DIR) 
    {
        return NULL;
    }
    FileNode *h = dir->childHead;
    if (!h) 
    {
        return NULL;
    }
    FileNode *p = h;
    do 
    {
        if (strcmp(p->name, name) == 0) 
        {
            return p;
        }
        p = p->next;
    } while (p && p != h);
    return NULL;
}

static void insert_child(FileNode *dir, FileNode *node) {
    if (!dir || dir->type != NODE_DIR || !node) 
    {
        return;
    }
    node->parent = dir;
    if (!dir->childHead) 
    {
        dir->childHead = node;
        node->next = node->prev = node;
    } 
    else 
    {
        FileNode *h = dir->childHead;
        FileNode *t = h->prev; 
        node->next = h;
        node->prev = t;
        t->next = node;
        h->prev = node;
    }
}

static void remove_child(FileNode *dir, FileNode *node) 
{
    if (!dir || dir->type != NODE_DIR || !node || !dir->childHead) 
    {
        return;
    }
    FileNode *h = dir->childHead;
    if (node->next == node && node->prev == node) 
    {
        dir->childHead = NULL;
    } 
    else 
    {
        node->prev->next = node->next;
        node->next->prev = node->prev;
        if (h == node) 
        {
            dir->childHead = node->next;
        }
    }
    node->next = node->prev = NULL;
    node->parent = NULL;
}

static bool dir_empty(FileNode *dir) 
{
    return !dir->childHead;
}

static void file_ensure_capacity(FileNode *f, int need) 
{
    if (f->file.blockCap >= need) 
    {
        return;
    }
    int newCap = f->file.blockCap ? f->file.blockCap : 4;
    while (newCap < need) 
    {
        newCap *= 2;
    }
    int *nb = (int*)realloc(f->file.blocks, newCap * sizeof(int));
    if (!nb) 
    { 
        perror("realloc"); 
        exit(1); 
    }
    f->file.blocks = nb;
    f->file.blockCap = newCap;
}

static size_t file_used_in_last_block(const FileNode *f) 
{
    if (f->file.blockCount == 0) 
    {
        return 0;
    }
    size_t rem = f->file.sizeBytes % BLOCK_SIZE;
    return rem;
}

static void file_append(FileNode *f, const unsigned char *data, size_t len) {
    if (!f || f->type != NODE_FILE) 
    {
        return;
    }
    size_t written = 0;
    size_t usedInLast = file_used_in_last_block(f);
    if (usedInLast > 0 && len > 0) 
    {
        int lastBlockIndex = f->file.blocks[f->file.blockCount - 1];
        size_t canWrite = BLOCK_SIZE - usedInLast;
        size_t chunk = (len < canWrite) ? len : canWrite;
        memcpy(diskBytes[lastBlockIndex] + usedInLast, data, chunk);
        written += chunk;
        f->file.sizeBytes += chunk;
    }

    while (written < len) 
    {
        int blk = freelist_pop_front();
        if (blk < 0) 
        {
            fprintf(stdout, "Disk full. Wrote %zu of %zu bytes.\n", written, len);
            fflush(stdout);
            return;
        }
        file_ensure_capacity(f, f->file.blockCount + 1);
        f->file.blocks[f->file.blockCount++] = blk;

        size_t remaining = len - written;
        size_t chunk = remaining < BLOCK_SIZE ? remaining : BLOCK_SIZE;
        memcpy(diskBytes[blk], data + written, chunk);
        
        if (chunk < BLOCK_SIZE) 
        {
            memset(diskBytes[blk] + chunk, 0, BLOCK_SIZE - chunk);
        }
        written += chunk;
        f->file.sizeBytes += chunk;
    }
}

static void file_read_to_stdout(const FileNode *f) 
{
    if (!f || f->type != NODE_FILE) 
    {
        return;
    }
    if (f->file.sizeBytes == 0) 
    {
        printf("(empty)\n");
        return;
    }
    size_t remaining = f->file.sizeBytes;
    int bi = 0;
    while (remaining > 0 && bi < f->file.blockCount) 
    {
        int blk = f->file.blocks[bi++];
        size_t chunk = remaining < BLOCK_SIZE ? remaining : BLOCK_SIZE;
        fwrite(diskBytes[blk], 1, chunk, stdout);
        remaining -= chunk;
    }
    fputc('\n', stdout);
}

static void file_return_blocks(FileNode *f) 
{
    if (!f || f->type != NODE_FILE) 
    {
        return;
    }
    for (int i = 0; i < f->file.blockCount; ++i) 
    {
        freelist_push_back(f->file.blocks[i]);
    }
    free_file_data(f);
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

static void build_path(FileNode *n, char *out, size_t outsz) {
    if (!n) 
    { 
        snprintf(out, outsz, "/"); 
        return; 
    }
    int depth = 0; FileNode *p = n;
    while (p && p != root) 
    { 
        depth++; 
        p = p->parent; 
    }
    
    const char **seg = (const char**)malloc(sizeof(char*) * (depth+1));
    int i = depth - 1; p = n;
    while (p && p != root) 
    { 
        seg[i--] = p->name; 
        p = p->parent; 
    }
    
    size_t pos = 0; out[0] = '\0';
    snprintf(out, outsz, "/"); pos = strlen(out);
    for (int k = 0; k < depth; ++k) 
    {
        size_t left = outsz - pos;
        int w = snprintf(out + pos, left, "%s%s", seg[k], (k+1<depth)?"/":"");
        if (w < 0 || (size_t)w >= left) 
        {
            break;
        }
        pos += (size_t)w;
    }
    free(seg);
}

static char *trim(char *s) 
{
    while (isspace((unsigned char)*s)) 
    {
        s++;
    }
    if (*s == '\0') 
    {
        return s;
    }
    char *e = s + strlen(s) - 1;
    while (e > s && isspace((unsigned char)*e)) 
    {
        *e-- = '\0';
    }
    return s;
}

static char* next_token(char **ps) 
{
    char *s = *ps; 
    if (!s) 
    {
        return NULL;
    }
    while (*s && isspace((unsigned char)*s)) 
    {
        s++;
    }
    if (!*s) 
    { 
        *ps = s; 
        return NULL; 
    }

    char *tok = NULL;
    if (*s == '"' || *s == '\'') 
    {
        char quote = *s++;
        tok = s; 
        while (*s && *s != quote) 
        {
            s++;
        }
        if (*s == quote) 
        {
            *s = '\0';
            s++;
        }
    } 
    else 
    {
        tok = s;
        while (*s && !isspace((unsigned char)*s)) 
        {
            s++;
        }
        if (*s) 
        { 
            *s = '\0'; 
            s++; 
        }
    }
    *ps = s;
    return tok;
}

static void cmd_ls(void) 
{
    if (!cwd->childHead) 
    { 
        printf("(empty)\n"); 
        return; 
    }
    FileNode *h = cwd->childHead, *p = h;
    do 
    {
        if (p->type == NODE_DIR) printf("%s/\n", p->name);
        else printf("%s\n", p->name);
        p = p->next;
    } while (p && p != h);
}

static void cmd_pwd(void) 
{
    char path[1024];
    build_path(cwd, path, sizeof(path));
    printf("%s\n", path);
}

static void cmd_df(void) 
{
    size_t used = (size_t)NUM_BLOCKS - freeCount;
    double pct = NUM_BLOCKS ? (100.0 * used / (double)NUM_BLOCKS) : 0.0;
    printf("Total Blocks: %d\n", NUM_BLOCKS);
    printf("Used Blocks: %zu\n", used);
    printf("Free Blocks: %zu\n", (size_t)freeCount);
    printf("Disk Usage: %.2f%%\n", pct);
}

static void cmd_mkdir(const char *name) 
{
    if (!name || !*name) 
    { 
        return; 
    }   
    if (find_child(cwd, name)) 
    { 
        printf("Name already exists in current directory.\n"); 
        return; 
    }
    FileNode *d = make_node(name, NODE_DIR);
    insert_child(cwd, d);
    printf("Directory '%s' created successfully.\n", name);
}

static void cmd_create(const char *name) 
{
    if (!name || !*name) 
    {
        return;
    }
    if (find_child(cwd, name)) 
    { 
        printf("Name already exists in current directory.\n"); 
        return; 
    }
    FileNode *f = make_node(name, NODE_FILE);
    insert_child(cwd, f);
    printf("File '%s' created successfully.\n", name);
}

static void cmd_write(const char *name, const char *content) 
{
    if (!name || !*name) 
    {
        return;
    }
    FileNode *f = find_child(cwd, name);
    if (!f || f->type != NODE_FILE) 
    {
        printf("File not found.\n"); 
        return; 
    }
    if (!content) 
    {
        content = "";
    }
    size_t len = strlen(content);
    file_append(f, (const unsigned char*)content, len);
    printf("Data written successfully (size=%zu bytes).\n", len);
}

static void cmd_read(const char *name) 
{
    if (!name || !*name) 
    {
        return;
    }
    FileNode *f = find_child(cwd, name);
    if (!f || f->type != NODE_FILE) 
    { 
        printf("File not found.\n"); 
        return; 
    }
    file_read_to_stdout(f);
}

static void cmd_delete(const char *name) 
{
    if (!name || !*name) 
    {
        return;
    }
    FileNode *f = find_child(cwd, name);
    if (!f || f->type != NODE_FILE)
    { 
        printf("File not found.\n"); 
        return; 
    }
    remove_child(cwd, f);
    file_return_blocks(f);
    free(f);
    printf("File deleted successfully.\n");
}

static void cmd_rmdir(const char *name) 
{
    if (!name || !*name) 
    {
        return;
    }
    FileNode *d = find_child(cwd, name);
    if (!d || d->type != NODE_DIR) 
    { 
        printf("Directory not found.\n"); 
        return; 
    }
    if (!dir_empty(d)) 
    { 
        printf("Directory not empty. Remove files first.\n"); 
        return; 
    }
    remove_child(cwd, d);
    free(d);
    printf("Directory removed successfully.\n");
}

static void cmd_cd(const char *name) 
{
    if (!name || !*name)
    {
        return;
    }
    if (strcmp(name, "/") == 0) 
    { 
        cwd = root; printf("Moved to /\n"); 
        return; 
    }
    if (strcmp(name, "..") == 0) 
    {
        if (cwd != root && cwd->parent) 
        { 
            cwd = cwd->parent; 
        }
        char path[1024]; build_path(cwd, path, sizeof(path));
        printf("Moved to %s\n", path);
        return;
    }
    FileNode *d = find_child(cwd, name);
    if (!d || d->type != NODE_DIR) 
    { 
        printf("Directory not found.\n"); 
        return; 
    }
    cwd = d;
    char path[1024];
    build_path(cwd, path, sizeof(path));
    printf("Moved to %s\n", path);
}

static void cmd_exit(void)
{
    if (root) 
    {
        FileNode *h = root->childHead;
        if (h) 
        {
            FileNode *p = h->next;
            h->next = NULL; 
            destroy_node_recursive(h);
            while (p) 
            {
                FileNode *nx = p->next;
                p->next = NULL; 
                destroy_node_recursive(p);
                p = nx;
            }
        }
        free(root);
        root = cwd = NULL;
    }
    disk_free();
    printf("Memory released. Exiting program...\n");
}

int main(void) 
{
    const char *env = getenv("VFS_BLOCKS");
    int blocks = DEFAULT_NUM_BLOCKS;
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
            if (!name) 
            {
                continue;
            }
            cmd_mkdir(name);
        } 
        else if (strcmp(cmd, "rmdir") == 0) 
        {
            char *name = next_token(&cursor);
            if (!name) 
            {
                continue;
            }
            cmd_rmdir(name);
        } 
        else if (strcmp(cmd, "create") == 0) 
        {
            char *name = next_token(&cursor);
            if (!name) 
            {
                continue;
            }
            cmd_create(name);
        } 
        else if (strcmp(cmd, "write") == 0) 
        {
            char *name = next_token(&cursor);
            char *content = next_token(&cursor);
            if (!name) 
            {
                continue;
            }
            cmd_write(name, content);
        } 
        else if (strcmp(cmd, "read") == 0) 
        {
            char *name = next_token(&cursor);
            if (!name) 
            {
                continue;
            }
            cmd_read(name);
        } 
        else if (strcmp(cmd, "delete") == 0) 
        {
            char *name = next_token(&cursor);
            if (!name) 
            {
                continue;
            }
            cmd_delete(name);
        } 
        else if (strcmp(cmd, "cd") == 0) 
        {
            char *name = next_token(&cursor);
            if (!name) 
            {
                continue;
            }
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