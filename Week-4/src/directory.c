#include "../inc/directory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FileNode* make_node(const char *name, NodeType type)
{
    FileNode *n = calloc(1, sizeof(FileNode));
    if (!n) { perror("calloc"); exit(1); }

    strncpy(n->name, name, NAME_MAX_LEN);
    n->name[NAME_MAX_LEN] = '\0';
    n->type = type;

    return n;
}

void destroy_node_recursive(FileNode *n)
{
    if (!n) return;

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
    free(n);
}

FileNode* find_child(FileNode *dir, const char *name)
{
    if (!dir || dir->type != NODE_DIR) return NULL;

    FileNode *h = dir->childHead;
    if (!h) return NULL;

    FileNode *p = h;
    do
    {
        if (strcmp(p->name, name) == 0)
            return p;

        p = p->next;
    } while (p && p != h);

    return NULL;
}

void insert_child(FileNode *dir, FileNode *node)
{
    if (!dir || !node || dir->type != NODE_DIR) return;

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

void remove_child(FileNode *dir, FileNode *node)
{
    if (!dir || !node || dir->type != NODE_DIR) return;
    if (!dir->childHead) return;

    FileNode *h = dir->childHead;

    if (node->next == node && node->prev == node)
        dir->childHead = NULL;
    else
    {
        node->prev->next = node->next;
        node->next->prev = node->prev;

        if (h == node)
            dir->childHead = node->next;
    }

    node->next = node->prev = NULL;
    node->parent = NULL;
}

bool dir_empty(FileNode *dir)
{
    return !dir->childHead;
}