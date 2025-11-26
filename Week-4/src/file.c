#include "../inc/file.h"
#include "../inc/directory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern unsigned char **diskBytes;
extern size_t freeCount;

extern int freelist_pop_front(void);
extern void freelist_push_back(int index);

static void file_ensure_capacity(FileNode *f, int need);
static size_t file_used_in_last_block(const FileNode *f);

void free_file_data(FileNode *f)
{
    if (!f || f->type != NODE_FILE) return;
    free(f->file.blocks);
    f->file.blocks = NULL;
    f->file.blockCap = f->file.blockCount = 0;
    f->file.sizeBytes = 0;
}

static void file_ensure_capacity(FileNode *f, int need)
{
    if (f->file.blockCap >= need) return;
    int newCap = f->file.blockCap ? f->file.blockCap : 4;
    while (newCap < need) newCap *= 2;

    int *nb = realloc(f->file.blocks, newCap * sizeof(int));
    if (!nb) { perror("realloc"); exit(1); }

    f->file.blocks = nb;
    f->file.blockCap = newCap;
}

static size_t file_used_in_last_block(const FileNode *f)
{
    if (f->file.blockCount == 0) return 0;
    return f->file.sizeBytes % BLOCK_SIZE;
}

void file_append(FileNode *f, const unsigned char *data, size_t len)
{
    if (!f || f->type != NODE_FILE) return;

    size_t written = 0;
    size_t used = file_used_in_last_block(f);

    if (used > 0 && len > 0)
    {
        int blk = f->file.blocks[f->file.blockCount - 1];
        size_t canWrite = BLOCK_SIZE - used;
        size_t chunk = (len < canWrite) ? len : canWrite;

        memcpy(diskBytes[blk] + used, data, chunk);

        written += chunk;
        f->file.sizeBytes += chunk;
    }

    while (written < len)
    {
        int blk = freelist_pop_front();
        if (blk < 0)
        {
            printf("Disk full. Wrote %zu of %zu bytes.\n", written, len);
            return;
        }

        file_ensure_capacity(f, f->file.blockCount + 1);
        f->file.blocks[f->file.blockCount++] = blk;

        size_t remaining = len - written;
        size_t chunk = remaining < BLOCK_SIZE ? remaining : BLOCK_SIZE;

        memcpy(diskBytes[blk], data + written, chunk);

        if (chunk < BLOCK_SIZE)
            memset(diskBytes[blk] + chunk, 0, BLOCK_SIZE - chunk);

        written += chunk;
        f->file.sizeBytes += chunk;
    }
}

void file_read_to_stdout(const FileNode *f)
{
    if (!f || f->type != NODE_FILE) return;

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
        size_t chunk = (remaining < BLOCK_SIZE) ? remaining : BLOCK_SIZE;

        fwrite(diskBytes[blk], 1, chunk, stdout);
        remaining -= chunk;
    }

    putchar('\n');
}

void file_return_blocks(FileNode *f)
{
    if (!f || f->type != NODE_FILE) return;

    for (int i = 0; i < f->file.blockCount; i++)
        freelist_push_back(f->file.blocks[i]);

    free_file_data(f);
}