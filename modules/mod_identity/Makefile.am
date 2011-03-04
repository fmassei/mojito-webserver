pkglib_LTLIBRARIES = mod_identity.la
mod_identity_la_SOURCES = mod_identity.c
mod_identity_la_LIBADD = ../../common_src/libcommon_src.la
if MOD_IDENTITY_STATIC
mod_identity_la_LDFLAGS = -static -module
else
mod_identity_la_LDFLAGS = -module -disable-static -no-undefined
endif
