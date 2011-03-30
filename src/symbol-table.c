/* vi: set sw=4 ts=4: */
/*
 * symbol-table.c
 *
 * This file is part of lmp-im.
 *
 * Copyright (C) 2011 - kelvenxu <kelvenxu@gmail.com>.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330,
 * Boston, MA 02111-1307, USA.
 * */

#include <stdio.h>
#include <stdlib.h>

#include "lmp-im-db.h"

enum {
	LINE_CHARS = 1024
};

int main(int argc, char *argv[])
{ 
	char line[LINE_CHARS];
	char code[32];
	char character[LINE_CHARS];
	int freq;

	FILE *fp = fopen("symbol.txt", "r");
	if(!fp)
	{
		fprintf(stderr, "Can't open symbol.txt\n");
		return -1;
	}

	if(db_open("wubi.db") == -1)
	{
		fprintf(stderr, "Can't open wubi.db\n");
		return -1;
	}

	db_table_create(DB_TABLE_SYMBOL);

	while(!feof(fp))
	{
		if(fgets(line, LINE_CHARS - 1, fp) == NULL)
			break;

		printf("line: %s\n", line);
		sscanf(line, "%s %s", code, character);
		printf("%s %s\n", code, character);

		CodeInfo info;
		info.code = code;
		info.chinese = character;
		info.freq = 0;

		db_insert(DB_TABLE_SYMBOL, &info);
	}

	db_close();
	fclose(fp);
	return 0;
}
