
INCLUDES = @GTK2_CFLAGS@
LDADDS = @GTK2_LIBS@
moduledir = $(libdir)/gtk-2.0/@GTK2_BINARY_VERSION@/immodules

module_LTLIBRARIES = im-lmp.la

im_lmp_la_SOURCES = \
										../src/lmp-im-object.c \
										../src/lmp-im-object.h \
										../src/lmp-im-window.c \
										../src/lmp-im-window.h \
										../src/lmp-im-db.c \
										../src/lmp-im-db.h \
										../src/lmp-im.c

im_lmp_la_CFLAGS = -g -DDATADIR=\""$(datadir)/lmp-im"\"	
im_lmp_la_LDFLAGS = -rpath $(moduledir) -avoid-version -module $(no_undefined)
im_lmp_la_LIBADD = @GTK2_LIBS@ -lsqlite3

bin_SCRIPTS = ../src/lmp-im-config

bin_PROGRAMS = lmpim-test-gtk2
noinst_PROGRAMS = wubi-table-gtk2 pinyin-table-gtk2 symbol-table-gtk2

lmpim_test_gtk2_SOURCES = ../src/lmpim-test.c
lmpim_test_gtk2_LDADD = $(LDADDS)

pinyin_table_gtk2_SOURCES = ../src/pinyin-table.c ../src/lmp-im-db.c
pinyin_table_gtk2_LDADD = $(LDADDS) -lsqlite3

wubi_table_gtk2_SOURCES = ../src/wubi-table.c ../src/lmp-im-db.c
wubi_table_gtk2_LDADD = $(LDADDS) -lsqlite3

symbol_table_gtk2_SOURCES = ../src/symbol-table.c ../src/lmp-im-db.c
symbol_table_gtk2_LDADD = $(LDADDS) -lsqlite3

