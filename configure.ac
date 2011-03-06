AC_INIT([mojito-webserver], [0.5], [fmassei@gmail.com])
AC_CONFIG_AUX_DIR([autotools])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CC
LT_INIT([shared win32-dll])
AC_CONFIG_HEADERS([config.h])

# Checks for libraries.
# Replace `main' with a function in `-ldl':
AC_CHECK_LIB([disml], [dis_parse_file])
# Replace `main' with a function in `-lrt':
AC_CHECK_LIB([mmp_base], [mmp_trace_print])

# Checks for header files.
AC_CHECK_HEADERS([mmp/mmp_files.h disml/dis_parse.h stdlib.h string.h])
AC_CHECK_HEADERS([sys/socket.h sys/sendfile.h])
AC_CHECK_HEADERS([sys/epoll.h])

#Checks for functions
AC_CHECK_FUNCS([sendfile])

# libmmp dependency
AM_LDFLAGS="$AM_LDFLAGS -lmmp_base -ldisml"
AC_SUBST([AM_LDFLAGS])

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

###############################################################################
# generic dynamic support
###############################################################################
AC_MSG_CHECKING([whether to be able to load dynamic modules])
AC_ARG_ENABLE([dynamicmods],
              [AS_HELP_STRING([--enable-dynamic],
                              [enable dynamic modules (def=yes)])],
              [dynamicmodit="$enableval"],
              [dynamicmodit=yes])
AC_MSG_RESULT([$dynamicmodit])
AM_CONDITIONAL([DISABLE_DYNAMIC_MODULES], [test x"$dynamicmodit" = x"yes"])
if test x"$dynamicmodit" != x"yes"; then
    AC_DEFINE([DISABLE_DYNAMIC_MODULES],[],[dynamic modules])
fi

###############################################################################
# specific modules
###############################################################################
# mod_identity
AC_MSG_CHECKING([whether to build mod_identity as dynamic])
AC_ARG_ENABLE([mod_identity_dyn],
              [AS_HELP_STRING([--enable-mod_identity-dynamic],
                              [dynamic mod_identity (def=no)])],
              [mod_identity_dynit="$enableval"],
              [mod_identity_dynit=no])
AC_MSG_RESULT([$mod_identity_dynit])
AM_CONDITIONAL([MOD_IDENTITY_STATIC], [test x"$mod_identity_dynit" != x"yes"])
if test x"$mod_identity_dynit" != x"yes"; then
    AC_DEFINE([MOD_IDENTITY_STATIC],[],[dynamic mod_identity])
fi
# mod_deflate
AC_MSG_CHECKING([whether to build mod_deflate as dynamic])
AC_ARG_ENABLE([mod_deflate_dyn],
              [AS_HELP_STRING([--enable-mod_deflate-dynamic],
                              [dynamic mod_deflate (def=no)])],
              [mod_deflate_dynit="$enableval"],
              [mod_deflate_dynit=no])
AC_MSG_RESULT([$mod_deflate_dynit])
AM_CONDITIONAL([MOD_DEFLATE_STATIC], [test x"$mod_deflate_dynit" != x"yes"])
if test x"$mod_deflate_dynit" != x"yes"; then
    AC_DEFINE([MOD_DEFLATE_STATIC],[],[dynamic mod_deflate])
fi
# mod_gzip
AC_MSG_CHECKING([whether to build mod_gzip as dynamic])
AC_ARG_ENABLE([mod_gzip_dyn],
              [AS_HELP_STRING([--enable-mod_gzip-dynamic],
                              [dynamic mod_gzip (def=no)])],
              [mod_gzip_dynit="$enableval"],
              [mod_gzip_dynit=no])
AC_MSG_RESULT([$mod_gzip_dynit])
AM_CONDITIONAL([MOD_GZIP_STATIC], [test x"$mod_gzip_dynit" != x"yes"])
if test x"$mod_gzip_dynit" != x"yes"; then
    AC_DEFINE([MOD_GZIP_STATIC],[],[dynamic mod_gzip])
fi

AC_CONFIG_FILES([
                 Makefile
                 src/Makefile
                 common_src/Makefile
                 modules/Makefile
                 modules/mod_identity/Makefile
                 modules/mod_deflate/Makefile
                 modules/mod_gzip/Makefile
                 ])
AC_OUTPUT
