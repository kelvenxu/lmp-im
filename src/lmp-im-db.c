
#include "lmp-im-db.h"
#include <sqlite3.h>
#include <glib.h>
#include <stdio.h>
#include <string.h>

static sqlite3 *handle;
static char* wubi_table_name = "wubi_all";
static char* pinyin_table_name = "pinyin_all";

gboolean
db_table_exist(const char *table_name)
{
	char *err;

	char *cmd = g_strdup_printf("select * from sqlite_master where type = 'table' and name = '%s'", table_name);
	int ret = sqlite3_exec(handle, cmd, NULL, NULL, &err);
	g_free(cmd);

	if(ret == SQLITE_OK && err == NULL)
		return TRUE;
	else
		return FALSE;
}

int
db_open(const char *filename)
{
	int ret;
	char *err;
	char *cmd;

	ret = sqlite3_open(filename, &handle);
	if(ret != SQLITE_OK)
	{
		fprintf(stderr, "Can't open file '%s'\n", filename);
		return -1;
	}

	return 0;
}

void
db_close()
{
	if(handle)
	{
		sqlite3_close(handle);
		handle = NULL;
	}
}

gboolean 
db_table_create(const char *table_name)
{
	char *err;

	// 如果表不存在，就先创建表
	char *cmd = g_strdup_printf("create table %s (code TEXT, chinese TEXT, freq INT)", table_name);

	int ret = sqlite3_exec(handle, cmd, NULL, NULL, &err);
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
db_insert(const char *table_name, CodeInfo *info)
{
	char *err;

	if(!info)
		return FALSE;

	char *cmd = g_strdup_printf("insert into %s (code, chinese, freq) values('%s', '%s', '%d')", 
			table_name, 
			info->code, 
			info->chinese,
			info->freq);

	int ret = sqlite3_exec(handle, cmd, NULL, NULL, &err);
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
db_delete(const char *table_name, CodeInfo *info)
{
	char *err;

	if(!info)
		return FALSE;

	char *cmd = g_strdup_printf("delete from %s where chinese = '%s'", table_name, info->chinese);

	int ret = sqlite3_exec(handle, cmd, NULL, NULL, &err);
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
db_delete_all(const char *table_name)
{
	char *err;

	char *cmd = g_strdup_printf("delete from %s", table_name);
	int ret = sqlite3_exec(handle, cmd, NULL, NULL, &err);
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
db_query(const char *table_name, const char *query, DBQueryCallback callback, gpointer user_data)
{
	char *cmd;
	char *err;

	if(!query)
		return -1;

	cmd = g_strdup_printf("select * from %s where code = '%%%s%%'\n", table_name, query);

	g_print("%s\n", cmd);
	int ret = sqlite3_exec(handle, cmd, callback, user_data, &err);
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

GPtrArray *
db_query_wubi(const char *code)
{
	if(!code)
		return NULL;

	char *cmd = g_strdup_printf("select * from %s where code like '%s%%'\n", wubi_table_name, code);

	sqlite3_stmt *stmt = NULL;
	int ret = sqlite3_prepare_v2(handle, cmd, strlen(cmd), &stmt, NULL);
	if(ret != SQLITE_OK)
	{
		g_print("sqlite3_prepare_v2 failed, return %d\n", ret);
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

	return array;
}

GPtrArray *
db_query_pinyin(const char *code)
{
	if(!code)
		return NULL;

	char *cmd = g_strdup_printf("select * from %s where code = '%s'\n", pinyin_table_name, code);

	sqlite3_stmt *stmt;
	sqlite3_prepare_v2(handle, cmd, strlen(cmd), &stmt, NULL);

	GPtrArray *array = g_ptr_array_new();
	g_ptr_array_set_free_func(array, (GDestroyNotify)g_free);

	int ret = sqlite3_step(stmt);

	while(ret == SQLITE_ROW)
	{
		const unsigned char *code = sqlite3_column_text(stmt, 0);
		const unsigned char *chinese = sqlite3_column_text(stmt, 1);

		gchar *p = (unsigned char *)chinese;
		gchar utf8_char[8];
		while(p && (*p != '\0'))
		{
			gunichar c = g_utf8_get_char(p);
			memset(utf8_char, 0, 8);
			g_unichar_to_utf8(c, utf8_char);

			CodeInfo *info = g_new0(CodeInfo, 1);
			info->code = g_strdup(code);
			info->chinese = g_strdup(utf8_char);
			info->freq = 0;
			g_ptr_array_add(array, info);

			p = g_utf8_next_char(p);
		}

		//g_print("chinese: %s\n", chinese);

		ret = sqlite3_step(stmt);
	}

	sqlite3_finalize(stmt);

	return array;
}

