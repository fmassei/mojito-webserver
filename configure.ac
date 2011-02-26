AC_INIT([mojito-webserver], [0.5], [fmassei@gmail.com])
AC_CONFIG_AUX_DIR([autotools])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CC
AC_CONFIG_HEADERS([config.h])

###############################################################################
# debug compilation support
###############################################################################
AC_MSG_CHECKING([whether to build with debug information])
AC_ARG_ENABLE([debug],
              [AS_HELP_STRING([--enable-debug],
                              [enable debug data generation (def=no)])],
              [debugit="$enableval"],
              [debugit=no])
AC_MSG_RESULT([$debugit])
if test x"$debugit" = x"yes"; then
    AC_DEFINE([DEBUG],[],[Debug Mode])
    AM_CXXFLAGS="$AM_CXXFLAGS -g -Wall -Werror -O0"
    AM_CFLAGS="$AM_CFLAGS -g -Wall -Werror -O0"
else
    AC_DEFINE([NDEBUG],[],[No-debug Mode])
    AM_CXXFLAGS="$AM_CXXFLAGS -O3"
    AM_CFLAGS="$AM_CFLAGS -O3"
fi
AC_SUBST([AM_CXXFLAGS])
AC_SUBST([AM_CFLAGS])

# libmmp dependency
AM_LDFLAGS="$AM_LDFLAGS -lmmp_base -ldisml"
AC_SUBST([AM_LDFLAGS])

AC_CONFIG_FILES([
                 Makefile
                 src/Makefile
                 ])
AC_OUTPUT
