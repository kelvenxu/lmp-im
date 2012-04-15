
#include "lmp-im-db.h"
#include <sqlite3.h>
#include <glib.h>
#include <stdio.h>
#include <string.h>

//static char* wubi_table_name = "wubi_all";
//static char* pinyin_table_name = "pinyin_all";
//static char* symbol_table_name = "symbol_all";


G_DEFINE_TYPE(LmpimDB, lmpim_db, G_TYPE_OBJECT);

#define LMPIM_DB_GET_PRIVATE(o)\
	(G_TYPE_INSTANCE_GET_PRIVATE((o), LMPIM_TYPE_DB, LmpimDBPrivate))

struct _LmpimDBPrivate
{
	sqlite3 *handle;
	int page_size;
	int offset;
};


static void
lmpim_db_dispose(LmpimDB *self)
{
}

static void
lmpim_db_finalize(LmpimDB *self)
{
}

static void
lmpim_db_init(LmpimDB *self)
{
	LmpimDBPrivate *priv = LMPIM_DB_GET_PRIVATE(self);
	priv->handle = NULL;

	priv->page_size = 3;
	priv->offset = 0;
}

static void
lmpim_db_class_init(LmpimDBClass *self_class)
{
	GObjectClass *object_class = G_OBJECT_CLASS(self_class);

	g_type_class_add_private(self_class, sizeof(LmpimDBPrivate));
	object_class->dispose = (void (*)(GObject *object)) lmpim_db_dispose;
	object_class->finalize = (void (*)(GObject *object)) lmpim_db_finalize;
}

gboolean
db_table_exist(LmpimDB *self, const char *table_name)
{
	char *err;

	LmpimDBPrivate *priv = LMPIM_DB_GET_PRIVATE(self);

	char *cmd = g_strdup_printf("select * from sqlite_master where type = 'table' and name = '%s'", table_name);
	int ret = sqlite3_exec(priv->handle, cmd, NULL, NULL, &err);
	g_free(cmd);

	if(ret == SQLITE_OK && err == NULL)
		return TRUE;
	else
		return FALSE;
}

db_reset(LmpimDB *self)
{
	LmpimDBPrivate *priv = LMPIM_DB_GET_PRIVATE(self);
	priv->page_size = 3;
	priv->offset = 0;
}

int
db_open(LmpimDB *self, const char *filename)
{
	int ret;
	char *err;
	char *cmd;

	LmpimDBPrivate *priv = LMPIM_DB_GET_PRIVATE(self);
	ret = sqlite3_open(filename, &(priv->handle));
	if(ret != SQLITE_OK)
	{
		fprintf(stderr, "Can't open file '%s'\n", filename);
		return -1;
	}

	return 0;
}

void
db_close(LmpimDB *self)
{
	LmpimDBPrivate *priv = LMPIM_DB_GET_PRIVATE(self);
	if(priv->handle)
	{
		sqlite3_close(priv->handle);
		priv->handle = NULL;
	}
}

gboolean 
db_table_create(LmpimDB *self, const char *table_name)
{
	char *err;

	LmpimDBPrivate *priv = LMPIM_DB_GET_PRIVATE(self);
	// 如果表不存在，就先创建表
	char *cmd = g_strdup_printf("create table %s (code TEXT, chinese TEXT, freq INT)", table_name);

	int ret = sqlite3_exec(priv->handle, cmd, NULL, NULL, &err);
	g_free(cmd);

	if(ret != SQLITE_OK)
	{
		if(err)
		{
			g_print("database error: %s\n", err);
			sqlite3_free(err);
			return FALSE;
		}
	}

	return TRUE;
}

gboolean
db_update_freq(LmpimDB *self, const char *table, CodeInfo *info)
{
	char *err;
	char *cmd;
	int ret;

	if(!self || !table || !info)
		return FALSE;

	cmd = g_strdup_printf("update %s set freq = '%d' where chinese = '%s'", 
			table,
			info->freq,
			info->chinese);

	LmpimDBPrivate *priv = LMPIM_DB_GET_PRIVATE(self);

	ret = sqlite3_exec(priv->handle, cmd, NULL, NULL, &err);
	if(ret != SQLITE_OK)
	{
		if(err)
		{
			g_print("database error: %s\n", err);
			sqlite3_free(err);
			return FALSE;
		}
	}

	g_free(cmd);
	return TRUE;
}

gboolean
db_update_wubi_freq(LmpimDB *self, CodeInfo *info)
{
	return db_update_freq (self, DB_TABLE_WUBI, info);
}

gboolean
db_update_pinyin_freq(LmpimDB *self, CodeInfo *info)
{
	return db_update_freq (self, DB_TABLE_PINYIN, info);
}

gboolean
db_insert(LmpimDB *self, const char *table_name, CodeInfo *info)
{
	char *err;

	if(!info)
		return FALSE;

	LmpimDBPrivate *priv = LMPIM_DB_GET_PRIVATE(self);

	char *cmd = g_strdup_printf("insert into %s (code, chinese, freq) values('%s', '%s', '%d')", 
			table_name, 
			info->code, 
			info->chinese,
			info->freq);

	int ret = sqlite3_exec(priv->handle, cmd, NULL, NULL, &err);
	if(ret != SQLITE_OK)
	{
		if(err)
		{
			g_print("database error: %s\n", err);
			sqlite3_free(err);
			return FALSE;
		}
	}

	g_free(cmd);
	return TRUE;
}

gboolean
db_delete(LmpimDB *self, const char *table_name, CodeInfo *info)
{
	char *err;

	if(!info)
		return FALSE;

	LmpimDBPrivate *priv = LMPIM_DB_GET_PRIVATE(self);

	char *cmd = g_strdup_printf("delete from %s where chinese = '%s'", table_name, info->chinese);

	int ret = sqlite3_exec(priv->handle, cmd, NULL, NULL, &err);
	g_free(cmd);

	if(ret != SQLITE_OK)
	{
		if(err)
		{
			g_print("database error: %s\n", err);
			sqlite3_free(err);
			return FALSE;
		}
	}

	return TRUE;
}

gboolean
db_delete_all(LmpimDB *self, const char *table_name)
{
	char *err;

	LmpimDBPrivate *priv = LMPIM_DB_GET_PRIVATE(self);

	char *cmd = g_strdup_printf("delete from %s", table_name);
	int ret = sqlite3_exec(priv->handle, cmd, NULL, NULL, &err);
	g_free(cmd);

	if(ret != SQLITE_OK)
	{
		if(err)
		{
			g_print("database error: %s\n", err);
			sqlite3_free(err);
			return FALSE;
		}
	}

	return TRUE;
}

int
db_query(LmpimDB *self, const char *table_name, const char *query, DBQueryCallback callback, gpointer user_data)
{
	char *cmd;
	char *err;

	if(!query)
		return -1;

	LmpimDBPrivate *priv = LMPIM_DB_GET_PRIVATE(self);

	cmd = g_strdup_printf("select * from %s where code = '%%%s%%'\n", table_name, query);

	g_print("%s\n", cmd);
	int ret = sqlite3_exec(priv->handle, cmd, callback, user_data, &err);
	if(ret != SQLITE_OK)
	{
		if(err)
		{
			g_print("database error: %s\n", err);
			sqlite3_free(err);
		}
	}

	g_free(cmd);

	return ret;
}

//按freq的大小从大到小排序，即將使用频率高放在最前面 
static gint
freq_compare(CodeInfo **c1, CodeInfo **c2)
{
  return ((*c2)->freq - (*c1)->freq);
}

void
db_query_next(LmpimDB *self)
{
	g_return_if_fail(LMPIM_IS_DB(self));
	LmpimDBPrivate *priv = LMPIM_DB_GET_PRIVATE(self);

	priv->offset += priv->page_size;
}

void
db_query_previous(LmpimDB *self)
{
	g_return_if_fail(LMPIM_IS_DB(self));
	LmpimDBPrivate *priv = LMPIM_DB_GET_PRIVATE(self);

	if(priv->offset >= 0)
		priv->offset -= priv->page_size;
}

GPtrArray *
db_query_wubi(LmpimDB *self, const char *code)
{
	g_return_val_if_fail(LMPIM_IS_DB(self), NULL);
	g_return_val_if_fail(code != NULL, NULL);

	LmpimDBPrivate *priv = LMPIM_DB_GET_PRIVATE(self);

	if(priv->offset < 0)
		return NULL;

	char *cmd = g_strdup_printf("select * from %s where code like '%s%%' limit %d, %d", 
			DB_TABLE_WUBI, 
			code, 
			priv->offset, 
			priv->page_size);

	sqlite3_stmt *stmt = NULL;
	int ret = sqlite3_prepare_v2(priv->handle, cmd, strlen(cmd), &stmt, NULL);
	if(ret != SQLITE_OK)
	{
		g_print("sqlite3_prepare_v2 failed, return %d\n", ret);
		return NULL;
	}

	GPtrArray *array = g_ptr_array_new();

	g_ptr_array_set_free_func(array, (GDestroyNotify)g_free);

	ret = sqlite3_step(stmt);

	while(ret == SQLITE_ROW)
	{
		CodeInfo *info = g_new0(CodeInfo, 1);

		const unsigned char *code = sqlite3_column_text(stmt, 0);
		info->code = g_strdup(code);

		const unsigned char *chinese = sqlite3_column_text(stmt, 1);
		info->chinese = g_strdup(chinese);

		info->freq = sqlite3_column_int(stmt, 2);

		g_ptr_array_add(array, info);

		ret = sqlite3_step(stmt);
	}

	sqlite3_finalize(stmt);

  g_ptr_array_sort(array, (GCompareFunc)freq_compare);

	return array;
}

// 反查汉字的五笔码
gchar *
db_query_wubi_code(LmpimDB *self, const gchar *chinese)
{
	if(!chinese)
		return NULL;

	gchar *code = NULL;

	LmpimDBPrivate *priv = LMPIM_DB_GET_PRIVATE(self);

	char *cmd = g_strdup_printf("select * from %s where chinese = '%s'\n", DB_TABLE_WUBI, chinese);

	sqlite3_stmt *stmt;
	sqlite3_prepare_v2(priv->handle, cmd, strlen(cmd), &stmt, NULL);

	int ret = sqlite3_step(stmt);

	if(ret == SQLITE_ROW)
	{
		const char *str = sqlite3_column_text(stmt, 0);

		if(str)
			code = g_strdup(str);
	}

	sqlite3_finalize(stmt);

	return code;
}

GPtrArray *
db_query_pinyin(LmpimDB *self, const char *code)
{
	int ret;
	char *cmd;
	sqlite3_stmt *stmt;
	GPtrArray *array;

	g_return_val_if_fail(LMPIM_IS_DB(self), NULL);
	g_return_val_if_fail(code != NULL, NULL);

	LmpimDBPrivate *priv = LMPIM_DB_GET_PRIVATE(self);

	if(priv->offset < 0)
		return NULL;

	cmd = g_strdup_printf("select * from %s where code = '%s' order by freq desc limit %d, %d\n", 
			DB_TABLE_PINYIN, 
			code, 
			priv->offset, 
			priv->page_size);

	sqlite3_prepare_v2(priv->handle, cmd, strlen(cmd), &stmt, NULL);

	array = g_ptr_array_new();
	g_ptr_array_set_free_func(array, (GDestroyNotify)g_free);

	ret = sqlite3_step(stmt);

	while(ret == SQLITE_ROW)
	{
		CodeInfo *info = g_new0(CodeInfo, 1);

		const unsigned char *code = sqlite3_column_text(stmt, 0);
		info->code = g_strdup(code);

		const unsigned char *chinese = sqlite3_column_text(stmt, 1);
		info->chinese = g_strdup(chinese);

		info->freq = sqlite3_column_int(stmt, 2);

		g_ptr_array_add(array, info);

		ret = sqlite3_step(stmt);
	}

	sqlite3_finalize(stmt);

	return array;
}

gchar *
db_query_symbol(LmpimDB *self, gchar code)
{
	if(!code)
		return NULL;

	gchar *chinese = NULL;

	LmpimDBPrivate *priv = LMPIM_DB_GET_PRIVATE(self);

	char *cmd = g_strdup_printf("select * from %s where code = '%c'\n", DB_TABLE_SYMBOL, code);

	sqlite3_stmt *stmt;
	sqlite3_prepare_v2(priv->handle, cmd, strlen(cmd), &stmt, NULL);

	int ret = sqlite3_step(stmt);

	if(ret == SQLITE_ROW)
	{
		const char *str = sqlite3_column_text(stmt, 1);

		if(str)
			chinese = g_strdup(str);
	}

	sqlite3_finalize(stmt);

	return chinese;
}
