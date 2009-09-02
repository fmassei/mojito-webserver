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
TARGET_CACHES = caches
TARGET_FILTERS = filters

#--------------------------
#debug or release
PSTATUS = debug
#lstatic, lshared or lnone
LOGGER = lshared
CACHE = lshared
FILTER = lshared

#--------------------------
ifeq ($(PSTATUS), debug)
	CFLAGS = -Wall -Wextra -O0 -ansi -pedantic -g
else
	CFLAGS = -Wall -Wextra -O3 -ansi -pedantic -DNDEBUG
endif

ifeq ($(CACHE), lstatic)
	CACHE_LNK = shm
	CACHE_LINK = -Lcache/$(CACHE_LNK) -lcache$(CACHE_LNK)
endif
ifeq ($(CACHE), lshared)
	DYNAMIC_LINKAGE := $(DYNAMIC_LINKAGE) -DDYNAMIC_CACHE
	DYNAMIC = -DDYNAMIC
	LIBS := $(LIBS) -ldl
endif
ifeq ($(CACHE), lnone)
	CACHE_LINK =
	MISSING_LINKAGE := $(MISSING_LINKAGE) -DNOCACHE
	TARGET_CACHES =
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



#--------------------------
OBJS=daemon.o date.o fileutils.o fparams.o main.o request.o response.o \
    socket.o mime.o cgi.o filter_manag.o header_w_quality.o \
	module.o plist.o cache/cache.o logger/logger.o filter/filter.o

all: $(TARGET_CACHES) $(TARGET_LOGGERS) $(TARGET_FILTERS) mojito

loggers:
ifeq ($(LOGGER), lnone)
	@echo "value 'lnone' for logger is not supported."
	@exit 2
endif
	@(cd logger && make)

caches:
	@(cd cache && make)

filters:
ifeq ($(FILTER), lnone)
	@echo "value 'lnone' for filter is not supported."
	@exit 2
endif
	@(cd filter && make)

mojito: $(OBJS)
	$(LD) $(OBJS) -o $(PNAME) $(LDFLAGS) $(LIBS) $(CACHE_LINK) $(LOGGER_LINK) $(FILTER_LINK)

.c.o:
	$(GCC) $(INCDIR) -c $< -o $@ $(CFLAGS) $(DYNAMIC) $(DYNAMIC_LINKAGE) $(MISSING_LINKAGE)

clean:
	-$(RM) $(OBJS)
	-$(RM) $(PNAME)
	@(cd logger && make clean)
	@(cd cache && make clean)
	@(cd filter && make clean)

