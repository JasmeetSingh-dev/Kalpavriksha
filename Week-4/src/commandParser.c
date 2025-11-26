#include "../inc/commandParser.h"
#include "../inc/file.h"
#include "../inc/directory.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

extern unsigned char **diskBytes;
extern int NUM_BLOCKS;
extern size_t freeCount;
extern void disk_free(void);
extern void freelist_push_back(int index);
extern int freelist_pop_front(void);

FileNode *root = NULL;
FileNode *cwd  = NULL;

void build_path(FileNode *n, char *out, size_t outsz)
{
    if (!n)
    {
        snprintf(out, outsz, "/");
        return;
    }
    int depth = 0;
    FileNode *p = n;
    while (p && p != root)
    {
        depth++;
        p = p->parent;
    }

    const char **seg = (const char**)malloc(sizeof(char*) * (depth + 1));
    int i = depth - 1;
    p = n;
    while (p && p != root)
    {
        seg[i--] = p->name;
        p = p->parent;
    }

    size_t pos = 0;
    out[0] = '\0';
    snprintf(out, outsz, "/");
    pos = strlen(out);

    for (int k = 0; k < depth; ++k)
    {
        size_t left = outsz - pos;
        int w = snprintf(out + pos, left, "%s%s", seg[k], (k + 1 < depth) ? "/" : "");
        if (w < 0 || (size_t)w >= left)
        {
            break;
        }
        pos += (size_t)w;
    }

    free(seg);
}

char *trim(char *s)
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

char* next_token(char **ps)
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


void cmd_ls(void)
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

void cmd_pwd(void)
{
    char path[1024];
    build_path(cwd, path, sizeof(path));
    printf("%s\n", path);
}

void cmd_df(void)
{
    size_t used = (size_t)NUM_BLOCKS - freeCount;
    double pct = NUM_BLOCKS ? (100.0 * used / (double)NUM_BLOCKS) : 0.0;
    printf("Total Blocks: %d\n", NUM_BLOCKS);
    printf("Used Blocks: %zu\n", used);
    printf("Free Blocks: %zu\n", (size_t)freeCount);
    printf("Disk Usage: %.2f%%\n", pct);
}

void cmd_mkdir(const char *name)
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

void cmd_create(const char *name)
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

void cmd_write(const char *name, const char *content)
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

void cmd_read(const char *name)
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

void cmd_delete(const char *name)
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

void cmd_rmdir(const char *name)
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

void cmd_cd(const char *name)
{
    if (!name || !*name)
    {
        return;
    }
    if (strcmp(name, "/") == 0)
    {
        cwd = root;
        printf("Moved to /\n");
        return;
    }
    if (strcmp(name, "..") == 0)
    {
        if (cwd != root && cwd->parent)
        {
            cwd = cwd->parent;
        }
        char path[1024];
        build_path(cwd, path, sizeof(path));
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

void cmd_exit(void)
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