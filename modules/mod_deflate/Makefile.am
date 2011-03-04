pkglib_LTLIBRARIES = mod_deflate.la
mod_deflate_la_SOURCES = mod_deflate.c
mod_deflate_la_LIBADD = ../../common_src/libcommon_src.la
if MOD_DEFLATE_STATIC
mod_deflate_la_LDFLAGS = -lz -static -module
else
mod_deflate_la_LDFLAGS = -lz -module -disable-static -no-undefined
endif
