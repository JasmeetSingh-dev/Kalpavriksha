#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include "directory.h"

extern FileNode *root;
extern FileNode *cwd;

void cmd_ls(void);
void cmd_pwd(void);
void cmd_df(void);

void cmd_mkdir(const char *name);
void cmd_create(const char *name);
void cmd_write(const char *name, const char *content);
void cmd_read(const char *name);
void cmd_delete(const char *name);
void cmd_rmdir(const char *name);
void cmd_cd(const char *name);
void cmd_exit(void);

char *trim(char *s);
char *next_token(char **ps);

#endif