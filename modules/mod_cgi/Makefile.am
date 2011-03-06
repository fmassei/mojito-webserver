pkglib_LTLIBRARIES = mod_cgi.la
mod_cgi_la_SOURCES = mod_cgi.c cgi_run.c
mod_cgi_la_LIBADD = ../../common_src/libcommon_src.la
if MOD_IDENTITY_STATIC
mod_cgi_la_LDFLAGS = -static -module
else
mod_cgi_la_LDFLAGS = -module -disable-static -no-undefined
endif
