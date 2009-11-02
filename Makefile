.PHONY: debug release all

include Makefile.flags

INCDIR  =   -I.

AFLAGS  =
PNAME   =   mojito
DYNAMIC =
DYNAMIC_LINKAGE = 
MISSING_LINKAGE =

# specify module-set targets
TARGET_MODS = mods

ifeq ($(MODULE), lstatic)
	MODULE_LINK = -Lmodules/mod_stat -lmod_stat -Lmodules/mod_cacheshm -lmod_cacheshm -Lmodules/mod_gzip -lmod_gzip -Lmodules/mod_deflate -lmod_deflate -Lmodules/mod_identity -lmod_identity -Lmodules/mod_cgi -l mod_cgi -lz -lrt
	#MODULE_LINK = -Lmodules/mod_stat -lmod_stat -Lmodules/mod_cacheshm -lmod_cacheshm -Lmodules/mod_gzip -lmod_gzip -Lmodules/mod_deflate -lmod_deflate -Lmodules/mod_identity -lmod_identity -Lmodules/mod_cgi -l mod_cgi -Lmodules/mod_fastcgi -l mod_fcgi -lz -lrt
	LDFLAGS =
endif
ifeq ($(MODULE), lstripped)
	MODULE_LINK = -Lmodules/mod_gzip -lmod_gzip -Lmodules/mod_deflate -lmod_deflate -Lmodules/mod_identity -lmod_identity -lz
	DYNAMIC_LINKAGE := $(DYNAMIC_LINKAGE) -DMOD_STRIPPED
	LDFLAGS =
endif
ifeq ($(MODULE), lshared)
	DYNAMIC_LINKAGE := $(DYNAMIC_LINKAGE) -DDYNAMIC_MODULE
	DYNAMIC = -DDYNAMIC
	LIBS = -ldl
	LDFLAGS = -Wl,-E
endif


#--------------------------
OBJS=daemon.o date.o fileutils.o fparams.o main.o request.o response.o \
    socket.o mime.o header_w_quality.o filter_manag.o \
	resp_headers.o plist.o logger.o module_loader.o \
	modules/modules.o

all: $(TARGET_MODS) mojito

mods:
ifeq ($(MODULE), lnone)
	@echo "value 'lnone' for module is not supported."
	@exit 2
endif
	@(cd modules && make)

mojito: $(OBJS)
	$(LD) $(OBJS) -o $(PNAME) $(LDFLAGS) $(LIBS) $(MODULE_LINK)
	$(STRIP) $(PNAME)

.c.o:
	$(GCC) $(INCDIR) -c $< -o $@ $(CFLAGS) $(DYNAMIC) $(DYNAMIC_LINKAGE) $(MISSING_LINKAGE)

clean:
	-$(RM) $(OBJS)
	-$(RM) $(PNAME)
	@(cd modules && make clean)

