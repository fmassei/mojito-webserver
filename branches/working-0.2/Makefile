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
TARGET_FILTERS = filters
TARGET_MODS = mods

#--------------------------
#debug or release
PSTATUS = debug
#lstatic, lshared or lnone
LOGGER = lshared
FILTER = lshared
MODULE = lshared

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

ifeq ($(FILTER), lstatic)
	FILTER_LINK = -Lfilter/identity -lfilteridentity -Lfilter/deflate -lfilterdeflate -Lfilter/gzip -lfiltergzip
endif
ifeq ($(FILTER), lshared)
	DYNAMIC_LINKAGE := $(DYNAMIC_LINKAGE) -DDYNAMIC_FILTER
	DYNAMIC = -DDYNAMIC
	LIBS := $(LIBS) -ldl
endif

ifeq ($(MODULE), lstatic)
	MODULE_LINK = -Lmodules/mod_stat -lmod_stat -Lmodules/mod_cacheshm -lmod_cacheshm
endif
ifeq ($(MODULE), lshared)
	DYNAMIC_LINKAGE := $(DYNAMIC_LINKAGE) -DDYNAMIC_MODULE
	DYNAMIC = -DDYNAMIC
	LIBS := $(LIBS) -ldl
endif


#--------------------------
OBJS=daemon.o date.o fileutils.o fparams.o main.o request.o response.o \
    socket.o mime.o cgi.o filter_manag.o header_w_quality.o \
	module.o plist.o logger/logger.o filter/filter.o \
	modules/modules.o

all: $(TARGET_LOGGERS) $(TARGET_FILTERS) $(TARGET_MODS) mojito

loggers:
ifeq ($(LOGGER), lnone)
	@echo "value 'lnone' for logger is not supported."
	@exit 2
endif
	@(cd logger && make)

filters:
ifeq ($(FILTER), lnone)
	@echo "value 'lnone' for filter is not supported."
	@exit 2
endif
	@(cd filter && make)

mods:
ifeq ($(MODULE), lnone)
	@echo "value 'lnone' for module is not supported."
	@exit 2
endif
	@(cd modules && make)

mojito: $(OBJS)
	$(LD) $(OBJS) -o $(PNAME) $(LDFLAGS) $(LIBS) $(LOGGER_LINK) $(FILTER_LINK) $(MODULE_LINK)

.c.o:
	$(GCC) $(INCDIR) -c $< -o $@ $(CFLAGS) $(DYNAMIC) $(DYNAMIC_LINKAGE) $(MISSING_LINKAGE)

clean:
	-$(RM) $(OBJS)
	-$(RM) $(PNAME)
	@(cd logger && make clean)
	@(cd filter && make clean)
	@(cd modules && make clean)

