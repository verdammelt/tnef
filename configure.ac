AC_PREREQ([2.72])
AC_INIT([tnef],[1.4.18],[verdammelt@gmail.com])
AC_CONFIG_SRCDIR([src/tnef.c])
AM_INIT_AUTOMAKE
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_LIBOBJ_DIR(src/replace)

dnl check for programs
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_RANLIB
AC_CHECK_TOOL(AR, ar)

dnl check for libraries
AC_CHECK_LIB([intl],gettext)

dnl check for headers
AC_CHECK_HEADERS([libintl.h])
AC_CHECK_DECLS([basename])

dnl check for typedefs
AC_TYPE_SIZE_T
AC_HEADER_STAT

dnl check for structures

dnl check for compiler characteristics.
AC_C_CONST
AC_CHECK_SIZEOF(int)

dnl check for library functions
AC_REPLACE_FUNCS(strdup getopt_long basename)
AC_CHECK_FUNCS(strrchr memmove memset strtoul index rindex)
AC_FUNC_VPRINTF
AC_FUNC_MALLOC

dnl do not use this, will try to add in lstat.c which is unnecessary
dnl AC_FUNC_STAT

dnl check for system services

dnl extra arguments:
AM_MAINTAINER_MODE
AC_ARG_ENABLE( warnings,
               [ --enable-warnings	enable compile warnings],
               AC_COMPILE_WARNINGS)
AC_ARG_ENABLE( debug,
               [ --enable-debug  enable debug build],
               AC_DEBUG_COMPILE)

dnl output
AC_CONFIG_FILES([Makefile BUGS tnef.spec
                 doc/Makefile doc/tnef.1 doc/www/Makefile
                 doc/www/cgi-bin/Makefile doc/www/htdocs/Makefile
                 src/Makefile src/replace/Makefile
                 tests/Makefile
                 tests/cmdline/Makefile
                 tests/files/Makefile
                 tests/files/baselines/Makefile
                 tests/files/datafiles/Makefile])

AC_OUTPUT
