bin_PROGRAMS = mojito
mojito_SOURCES =    config_manager.c main.c request.c \
                    config_parse.c mime.c request_parse.c socket_unit.c \
                    filter_manager.c module_loader.c header_codes.c \
                    header_w_quality.c modules.c response.c logger.c \
                    scheduler_epoll.c scheduler_fake.c lptask.c
mojito_LDADD = ../common_src/libcommon_src.la
if MOD_IDENTITY_STATIC
mojito_LDADD += ../modules/mod_identity/mod_identity.la
endif
if MOD_DEFLATE_STATIC
mojito_LDADD += ../modules/mod_deflate/mod_deflate.la
endif
if MOD_GZIP_STATIC
mojito_LDADD += ../modules/mod_gzip/mod_gzip.la
endif
if MOD_CGI_STATIC
mojito_LDADD += ../modules/mod_cgi/mod_cgi.la
endif
