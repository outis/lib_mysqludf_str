### Library to create.  This is used by libtool and turns into a .so or .dll
### during the "make install"
###
lib_LTLIBRARIES = lib_mysqludf_str.la

### Source files used by this project.  Note the prefix to SOURCES is the
### target library.
###
lib_mysqludf_str_la_SOURCES = lib_mysqludf_str.c

###
### This defines the CFLAGS that are passed to the compiler.  The variables
### surrouned by @ are set by the ".m4" files included from the acinclude.m4
### described above.  The -DSTANDARD is so that config.h does not conflict
### with my_config.h.
###
lib_mysqludf_str_la_CFLAGS = -DSTANDARD -DMYSQL_SERVER @MYSQL_CFLAGS@

### The LDFLAGS passed to the linker.
lib_mysqludf_str_la_LDFLAGS = -module -avoid-version -no-undefined @MYSQL_LDFLAGS@

# This next thing should be set by an "m4" file.  Unfortunately,
# The version of ax_prog_mysql.m4 that I found did not do this
# properly.  We will eventually need to write a more advanced mysql.m4 for
# this udf repository, which should set the location of the mysql binary.
#
#MYSQL=mysql

### Standard Make target, which will be placed verbatim into the Makefile
###
installdb: uninstalldb
	$(MYSQL) <./installdb.sql

#### Another standard Make target
####
uninstalldb:
	$(MYSQL) <./uninstalldb.sql

mrproper:
	make clean
	make maintainer-clean
	rm -rf config.guess config.h.* config.status configure missing config.sub ltmain.sh depcomp aclocal.m4 install.sh config.log compile Makefile.in *.loT mkinstalldirs install-sh *~

