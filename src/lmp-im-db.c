
#include "lmp-im-db.h"
#include <sqlite3.h>
#include <glib.h>
#include <stdio.h>
#include <string.h>

static sqlite3 *handle;
static char* table_name = "wubi_all";
static char* pinyin_table_name = "pinyin_all";

#if 0
static gboolean
db_table_exist()
{
	int ret;
	char *err;
	char *cmd;

	// 判断表是否存在
	cmd = g_strdup_printf("select * from sqlite_master where type = 'table' and name = '%s'", table_name);
	ret = sqlite3_exec(handle, cmd, NULL, NULL, &err);
	g_free(cmd);

	if(ret == SQLITE_OK && err == NULL)
		return 0;
}
#endif

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

#if 0
	// 判断表是否存在
	cmd = g_strdup_printf("select * from sqlite_master where type = 'table' and name = '%s'", table_name);
	ret = sqlite3_exec(handle, cmd, NULL, NULL, &err);
	g_free(cmd);

	if(ret == SQLITE_OK && err == NULL)
		return 0;
#endif

	// 如果表不存在，就先创建表
	cmd = g_strdup_printf("create table %s (code TEXT, chinese TEXT, freq INT)", table_name);

	ret = sqlite3_exec(handle, cmd, NULL, NULL, &err);
	g_free(cmd);

	if(ret != SQLITE_OK)
	{
		if(err)
		{
			g_print("database error: %s\n", err);
			sqlite3_free(err);
		}
	}

	cmd = g_strdup_printf("create table %s (code TEXT, chinese TEXT, freq INT)", pinyin_table_name);

	ret = sqlite3_exec(handle, cmd, NULL, NULL, &err);
	g_free(cmd);

	if(ret != SQLITE_OK)
	{
		if(err)
		{
			g_print("database error: %s\n", err);
			sqlite3_free(err);
		}
	}

	return 0;
}

int 
db_insert(CodeInfo *info)
{
	char *cmd;
	int ret;
	char *err;

	if(!info)
		return -1;

	cmd = g_strdup_printf("insert into %s (code, chinese, freq) values('%s', '%s', '%d')", 
			table_name, 
			info->code, 
			info->chinese,
			info->freq);

	ret = sqlite3_exec(handle, cmd, NULL, NULL, &err);
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

int 
db_pinyin_insert(CodeInfo *info)
{
	char *cmd;
	int ret;
	char *err;

	if(!info)
		return -1;

	cmd = g_strdup_printf("insert into %s (code, chinese, freq) values('%s', '%s', '%d')", 
			pinyin_table_name, 
			info->code, 
			info->chinese,
			info->freq);

	ret = sqlite3_exec(handle, cmd, NULL, NULL, &err);
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

int
db_delete(CodeInfo *info)
{
	char *cmd;
	int ret;
	char *err;

	if(!info)
		return -1;

	cmd = g_strdup_printf("delete from %s where chinese = '%s'", table_name, info->chinese);

	ret = sqlite3_exec(handle, cmd, NULL, NULL, &err);
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

int
db_delete_all()
{
	char *cmd;
	int ret;
	char *err;

	cmd = g_strdup_printf("delete from %s", table_name);

	ret = sqlite3_exec(handle, cmd, NULL, NULL, &err);
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

int
db_query(const char *query, DBQueryCallback callback, gpointer user_data)
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
db_query2(const char *code)
{
	char *cmd;
	//char *err;
	int ret;

	if(!code)
		return NULL;

	//cmd = g_strdup_printf("select * from %s where code like '%%%s%%'\n", table_name, code);
	cmd = g_strdup_printf("select * from %s where code like '%s%%'\n", table_name, code);

	sqlite3_stmt *stmt;
	sqlite3_prepare_v2(handle, cmd, strlen(cmd), &stmt, NULL);

	//int cols;
	//char **value;
	//char **col_name;
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

		//g_print("chinese: %s\n", chinese);

		ret = sqlite3_step(stmt);
	}

	sqlite3_finalize(stmt);

	return array;
}

void
db_close()
{
	sqlite3_close(handle);
}

