AM_CPPFLAGS = -I $(srcdir)/include

AM_CFLAGS = -O3 -g0 -Wall

ARFLAGS = cr

bin_PROGRAMS = bin/gensym
bin_gensym_SOURCES = src/api.c src/attribute.c src/common.c src/debug.c src/gensym.c src/lihata_helpers.c src/parse.c src/pinalign.c src/pin.c src/pin_side_lists.c src/shape.c src/slot.c src/symbol.c src/uuid.c
bin_gensym_LDADD = -llihata -lgenht -lminuid

include_HEADERS = include/api.h include/attribute.h include/common.h include/debug.h include/lihata_helpers.h include/parse.h include/pinalign.h include/pin.h include/pin_side_lists.h include/shape.h include/slot.h include/symbol.h include/uuid.h

EXTRA_DIST = man acdoxygen.m4 amdoxygen.am doxygen.gensym.cfg .gitignore .gitattributes

man1_MANS = man/gensym.1
dist_man_man1_MANS = man/gensym.1

super-clean: clean distclean
	@find . -name 'Makefile.in' -delete
	@find . -name 'config.h.in' -delete
	@find . -name '*.tar' -delete
	@find . -name '*.tar.*' -delete
	@find . -name '*~' -delete
	@rm -rf autom4te.cache
	@rm -f aclocal.m4
	@rm -f configure
	@rm -rf src/.deps
	@rm -f texput.log

include amdoxygen.am

MOSTLYCLEANFILES = $(DX_CLEANFILES)

EXTRA_DIST += $(DX_CONFIG)

install-data-hook:
	@if test -d $(DX_HTML_DIR); \
	 then \
	   echo installing $(DX_HTML_DIR) to $(prefix)/share/doc/gensym; \
	   mkdir -p $(prefix)/share/doc/gensym; \
	   cp -r $(DX_HTML_DIR) $(prefix)/share/doc/gensym; \
	 fi
	@echo installing gensym.pc
	@mkdir -p $(prefix)/share/pkgconfig
	@cp gensym.pc $(prefix)/share/pkgconfig/gensym.pc

uninstall-hook:
	@rm -rf $(prefix)/share/doc/gensym
	@rm -rf $(prefix)/share/pkgconfig/gensym.pc

code-stats:
	@echo -n " total lines of source code: "; wc -l src/*.[ch] include/*.[ch] | tail -1 | sed -e 's/ total//' -e 's/^  *//'
	@echo -n "actual lines of source code: "; (gcc -fpreprocessed -dD -E src/*.[ch] include/*.[ch] | sed -e '/^$$/d' -e '/^# [0-9]/d' | wc -l)

