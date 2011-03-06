pkglib_LTLIBRARIES = mod_gzip.la
mod_gzip_la_SOURCES = mod_gzip.c
mod_gzip_la_LIBADD = ../../common_src/libcommon_src.la
if MOD_GZIP_STATIC
mod_gzip_la_LDFLAGS = -lz -static -module
else
mod_gzip_la_LDFLAGS = -lz -module -disable-static -no-undefined
endif
