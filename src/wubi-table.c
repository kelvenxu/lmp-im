/* vi: set sw=4 ts=4: */
/*
 * wubi-table.c
 *
 * This file is part of lmp-im.
 *
 * Copyright (C) 2010 - kelvenxu <kelvenxu@gmail.com>.
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
	LINE_CHARS = 256
};

int main(int argc, char *argv[])
{ 
	char line[LINE_CHARS];
	char code[32];
	char character[64];
	int freq;

	FILE *fp = fopen("wubi.txt", "r");
	db_open("lmp-table.db");

	while(!feof(fp))
	{
		if(fgets(line, LINE_CHARS - 1, fp) == NULL)
			break;

		//printf("line: %s\n", line);
		sscanf(line, "%s %s %d", code, character, &freq);
		//printf("%s %s %d\n", code, character, freq);

		CodeInfo info;
		info.code = code;
		info.chinese = character;
		info.freq = freq;
		db_insert(DB_TABLE_WUBI, &info);
	}

	db_close();
	fclose(fp);
	return 0;
}
