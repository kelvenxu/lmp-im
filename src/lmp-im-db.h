#ifndef __LMP_IM_DB__H__
#define __LMP_IM_DB__H__  1

#include "type.h"
#include <glib.h>

#define DB_TABLE_WUBI "wubi_table_name"
#define DB_TABLE_PINYIN "pinyin_table_name"

typedef int (*DBQueryCallback) (void*, int, char**, char**);

int db_open(const char *filename);
void db_close();

int db_table_create(const char *table_name);
gboolean db_table_exist(const char *table_name);

int db_insert(const char *table_name, CodeInfo *fileinfo);
gboolean db_delete(const char *table_name, CodeInfo *fileinfo);
gboolean db_delete_all(const char *table_name);

int db_query(const char *table_name, const char *query, DBQueryCallback callback, gpointer user_data);

GPtrArray * db_query_wubi(const char *code);
GPtrArray * db_query_pinyin(const char *code);

#endif /*__LMP_IM_DB__H__ */
