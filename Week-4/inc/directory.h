#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <stddef.h>
#include <stdbool.h>

#define BLOCK_SIZE 512
#define NAME_MAX_LEN 50

typedef enum {
    NODE_DIR = 1,
    NODE_FILE = 2
} NodeType;

typedef struct FileData {
    size_t sizeBytes;
    int *blocks;
    int blockCount;
    int blockCap;
} FileData;

typedef struct FileNode {
    char name[NAME_MAX_LEN + 1];
    NodeType type;
    struct FileNode *childHead;
    struct FileNode *next;
    struct FileNode *prev;
    struct FileNode *parent;
    FileData file;
} FileNode;

FileNode* make_node(const char *name, NodeType type);
FileNode* find_child(FileNode *dir, const char *name);
void insert_child(FileNode *dir, FileNode *node);
void remove_child(FileNode *dir, FileNode *node);
bool dir_empty(FileNode *dir);
void destroy_node_recursive(FileNode *n);

#endif