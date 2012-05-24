#ifndef __TYPE_H__
#define __TYPE_H__  1

#include <glib.h>

#define FILE_INFORMATION_N 7

typedef struct _CodeInfo
{
	char *code;
	char *chinese;
	int freq;
} CodeInfo;

typedef enum
{
	LMP_IM_MODE_ENGLISH = 0,
	LMP_IM_MODE_WUBI,
	LMP_IM_MODE_WUBI_ENGLISH,
	LMP_IM_MODE_PINYIN
} LmpIMMode;

#endif /*__TYPE_H__ */
