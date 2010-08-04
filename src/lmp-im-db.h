#ifndef __LMP_IM_DB__H__
#define __LMP_IM_DB__H__  1

#include "type.h"
#include <glib.h>

typedef int (*DBQueryCallback) (void*, int, char**, char**);

int db_open(const char *filename);
int db_insert(CodeInfo *fileinfo);
int db_delete(CodeInfo *fileinfo);
int db_delete_all();
int db_query(const char *query, DBQueryCallback callback, gpointer user_data);
GPtrArray * db_query2(const char *code);
void db_close();

#endif /*__LMP_IM_DB__H__ */
