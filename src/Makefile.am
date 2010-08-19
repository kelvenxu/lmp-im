
INCLUDES = @GTK2_CFLAGS@
LDADDS = @GTK2_LIBS@
moduledir = $(libdir)/gtk-2.0/@GTK2_BINARY_VERSION@/immodules

module_LTLIBRARIES = im-lmp.la

im_lmp_la_SOURCES = \
										lmp-im-object.c \
										lmp-im-object.h \
										lmp-im-window.c \
										lmp-im-window.h \
										lmp-im-db.c \
										lmp-im-db.h \
										lmp-im.c

im_lmp_la_CFLAGS = -g -DDATADIR=\""$(datadir)/lmp-im"\"	
im_lmp_la_LDFLAGS = -rpath $(moduledir) -avoid-version -module $(no_undefined)
im_lmp_la_LIBADD = @GTK2_LIBS@ -lsqlite3

bin_SCRIPTS = lmp-im-config

noinst_PROGRAMS = test-view wubi-table pinyin-table

test_view_SOURCES = test-view.c
test_view_LDADD = $(LDADDS)

pinyin_table_SOURCES = pinyin-table.c lmp-im-db.c
pinyin_table_LDADD = $(LDADDS) -lsqlite3

wubi_table_SOURCES = wubi-table.c lmp-im-db.c
wubi_table_LDADD = $(LDADDS) -lsqlite3

