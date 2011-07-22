#ifndef __LMP_IM_DB__H__
#define __LMP_IM_DB__H__  1

#include "type.h"
#include <glib.h>

#define DB_TABLE_WUBI "wubi_all"
#define DB_TABLE_PINYIN "pinyin_all"
#define DB_TABLE_SYMBOL "symbol_all"

#include <glib-object.h>

G_BEGIN_DECLS

#define LMPIM_TYPE_DB (lmpim_db_get_type())
#define LMPIM_DB(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), LMPIM_TYPE_DB, LmpimDB))
#define LMPIM_DB_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), LMPIM_TYPE_DB, LmpimDBClass))
#define LMPIM_IS_DB(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), LMPIM_TYPE_DB))
#define LMPIM_IS_DB_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), LMPIM_TYPE_DB))
#define LMPIM_DB_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), LMPIM_TYPE_DB, LmpimDBClass))

typedef struct _LmpimDBPrivate LmpimDBPrivate;

typedef struct 
{
	GObject parent;

	LmpimDBPrivate *priv;
} LmpimDB;

typedef struct
{
	GObjectClass parent;
} LmpimDBClass;


typedef int (*DBQueryCallback) (void*, int, char**, char**);

int db_open(LmpimDB *self, const char *filename);
void db_close(LmpimDB *self);

int db_table_create(LmpimDB *self, const char *table_name);
gboolean db_table_exist(LmpimDB *self, const char *table_name);

int db_insert(LmpimDB *self, const char *table_name, CodeInfo *fileinfo);
gboolean db_delete(LmpimDB *self, const char *table_name, CodeInfo *fileinfo);
gboolean db_delete_all(LmpimDB *self, const char *table_name);

int db_query(LmpimDB *self, const char *table_name, const char *query, DBQueryCallback callback, gpointer user_data);

GPtrArray * db_query_wubi(LmpimDB *self, const char *code);
GPtrArray * db_query_pinyin(LmpimDB *self, const char *code);
gchar * db_query_symbol(LmpimDB *self, char code);
gchar * db_query_wubi_code(LmpimDB *self, const gchar *chinese);

void db_query_next(LmpimDB *self);
void db_query_previous(LmpimDB *self);

gboolean db_update_freq(LmpimDB *self, const char *table, CodeInfo *info);
gboolean db_update_wubi_freq(LmpimDB *self, CodeInfo *info);
gboolean db_update_pinyin_freq(LmpimDB *self, CodeInfo *info);

G_END_DECLS

#endif /*__LMP_IM_DB__H__ */
