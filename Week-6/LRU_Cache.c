#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_SIZE 2003
#define MAX_VALUE_LEN 100

typedef struct Node 
{
    int key;
    char value[MAX_VALUE_LEN];
    struct Node *prev, *next;
} Node;

typedef struct HashNode 
{
    int key;
    Node *addr;
    struct HashNode *next;
} HashNode;

typedef struct LRUCache 
{
    int capacity;
    int size;
    Node *head;
    Node *tail;
    HashNode *map[HASH_SIZE];
} LRUCache;

int hashFunc(int key) 
{
    return (key % HASH_SIZE + HASH_SIZE) % HASH_SIZE;
}

Node* hashmapGet(LRUCache *cache, int key) 
{
    int idx = hashFunc(key);
    HashNode *curr = cache->map[idx];
    while (curr) 
    {
        if (curr->key == key) 
        {
            return curr->addr;
        }
        curr = curr->next;
    }
    return NULL;
}

void hashmapPut(LRUCache *cache, int key, Node *addr) 
{
    int idx = hashFunc(key);
    HashNode *entry = malloc(sizeof(HashNode));
    entry->key = key;
    entry->addr = addr;
    entry->next = cache->map[idx];
    cache->map[idx] = entry;
}

void hashmapDelete(LRUCache *cache, int key) 
{
    int idx = hashFunc(key);
    HashNode *curr = cache->map[idx], *prev = NULL;
    while (curr) 
    {
        if (curr->key == key)  
        {
            if (prev) 
            {   
                prev->next = curr->next;
            } 
            else 
            {
                cache->map[idx] = curr->next;
            }
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void removeNode(LRUCache *cache, Node *node) 
{
    if (!node) 
    {
        return;
    }

    if (node->prev) 
    {
        node->prev->next = node->next;
    }
    else 
    {
        cache->head = node->next;
    }

    if (node->next) 
    {
        node->next->prev = node->prev;
    } 
    else 
    {
        cache->tail = node->prev;
    }
}

void addToFront(LRUCache *cache, Node *node) 
{
    node->prev = NULL;
    node->next = cache->head;
    if (cache->head) 
    {
        cache->head->prev = node;
    }
    cache->head = node;
    if (!cache->tail) 
    {
        cache->tail = node;
    }
}

LRUCache* createCache(int capacity) 
{
    LRUCache *cache = malloc(sizeof(LRUCache));
    cache->capacity = capacity;
    cache->size = 0;
    cache->head = cache->tail = NULL;
    for (int i = 0; i < HASH_SIZE; i++) 
    {
        cache->map[i] = NULL;
    }
    return cache;
}

char* get(LRUCache *cache, int key) 
{
    Node *node = hashmapGet(cache, key);
    if (!node) 
    {
        return NULL;
    }
    removeNode(cache, node);
    addToFront(cache, node);
    return node->value;
}

void put(LRUCache *cache, int key, char *value) 
{
    Node *node = hashmapGet(cache, key);

    if (node) 
    {
        strcpy(node->value, value);
        removeNode(cache, node);
        addToFront(cache, node);
        return;
    }

    if (cache->size == cache->capacity) 
    {
        hashmapDelete(cache, cache->tail->key);
        Node *oldTail = cache->tail;
        removeNode(cache, oldTail);
        free(oldTail);
        cache->size--;
    }

    Node *newNode = malloc(sizeof(Node));
    newNode->key = key;
    strcpy(newNode->value, value);

    addToFront(cache, newNode);
    hashmapPut(cache, key, newNode);
    cache->size++;
}

void run() 
{
    LRUCache *cache = NULL;
    char command[50];

    while (scanf("%s", command) != EOF) 
    {
        if (!strcmp(command, "createCache")) 
        {
            int size;
            scanf("%d", &size);
            cache = createCache(size);
        } 
        else if (!strcmp(command, "put")) 
        {
            int key;
            char data[MAX_VALUE_LEN];
            scanf("%d %s", &key, data);
            put(cache, key, data);
        } 
        else if (!strcmp(command, "get")) 
        {
            int key;
            scanf("%d", &key);
            char *res = get(cache, key);
            if (res) 
            {
                printf("%s\n", res);
            } 
            else 
            {
                printf("NULL\n");
            }
        } 
        else if (!strcmp(command, "exit")) return;
    }
}

int main() 
{
    run();
    return 0;
}
