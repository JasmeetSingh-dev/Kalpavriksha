#ifndef FILE_H
#define FILE_H

#include <stddef.h>
#include "directory.h"

void file_append(FileNode *f, const unsigned char *data, size_t len);
void file_read_to_stdout(const FileNode *f);
void file_return_blocks(FileNode *f);
void free_file_data(FileNode *f);

#endif