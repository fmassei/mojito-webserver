.PHONY: debug release all

GCC =   gcc
LD  =   gcc
RM  =   rm -f

INCDIR  =   -I.
LIBS	:=   -lz -lrt
LDFLAGS = -Wl,-E

AFLAGS  =
PNAME   =   mojito
DYNAMIC =
DYNAMIC_LINKAGE = 
MISSING_LINKAGE =

# specify module-set targets
TARGET_LOGGERS = loggers
TARGET_MODS = mods

#--------------------------
#debug or release
PSTATUS = debug
#lstatic, lshared or lnone
LOGGER = lstatic
MODULE = lstatic

#--------------------------
ifeq ($(PSTATUS), debug)
	CFLAGS = -Wall -Wextra -O0 -ansi -pedantic -g
else
	CFLAGS = -Wall -Wextra -O3 -ansi -pedantic -DNDEBUG
endif

ifeq ($(LOGGER), lstatic)
	LOGGER_LNK = std
	LOGGER_LINK = -Llogger/$(LOGGER_LNK) -llogger$(LOGGER_LNK)
endif
ifeq ($(LOGGER), lshared)
	DYNAMIC_LINKAGE := $(DYNAMIC_LINKAGE) -DDYNAMIC_LOGGER
	DYNAMIC = -DDYNAMIC
	LIBS := $(LIBS) -ldl
endif
ifeq ($(LOGGER), lnone)
endif

ifeq ($(MODULE), lstatic)
	MODULE_LINK = -Lmodules/mod_stat -lmod_stat -Lmodules/mod_cacheshm -lmod_cacheshm -Lmodules/mod_gzip -lmod_gzip -Lmodules/mod_deflate -lmod_deflate -Lmodules/mod_identity -lmod_identity
endif
ifeq ($(MODULE), lshared)
	DYNAMIC_LINKAGE := $(DYNAMIC_LINKAGE) -DDYNAMIC_MODULE
	DYNAMIC = -DDYNAMIC
	LIBS := $(LIBS) -ldl
endif


#--------------------------
OBJS=daemon.o date.o fileutils.o fparams.o main.o request.o response.o \
    socket.o mime.o cgi.o header_w_quality.o filter_manag.o \
	resp_headers.o module.o plist.o logger/logger.o \
	modules/modules.o

all: $(TARGET_LOGGERS) $(TARGET_MODS) mojito

loggers:
ifeq ($(LOGGER), lnone)
	@echo "value 'lnone' for logger is not supported."
	@exit 2
endif
	@(cd logger && make)

mods:
ifeq ($(MODULE), lnone)
	@echo "value 'lnone' for module is not supported."
	@exit 2
endif
	@(cd modules && make)

mojito: $(OBJS)
	$(LD) $(OBJS) -o $(PNAME) $(LDFLAGS) $(LIBS) $(LOGGER_LINK) $(MODULE_LINK)

.c.o:
	$(GCC) $(INCDIR) -c $< -o $@ $(CFLAGS) $(DYNAMIC) $(DYNAMIC_LINKAGE) $(MISSING_LINKAGE)

clean:
	-$(RM) $(OBJS)
	-$(RM) $(PNAME)
	@(cd logger && make clean)
	@(cd modules && make clean)

