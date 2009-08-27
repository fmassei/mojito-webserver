.PHONY: debug release all

GCC =   gcc
LD  =   gcc
RM  =   rm -f

INCDIR  =   -I.
LIBS	:=   -lz -lrt

AFLAGS  =
PNAME   =   mojito
DYNAMIC =
DYNAMIC_LINKAGE = 

#--------------------------
#debug or release
PSTATUS = debug
#lstatic or lshared
LOGGER = lstati
CACHE = lstati

#--------------------------
ifeq ($(PSTATUS), debug)
	CFLAGS = -Wall -Wextra -O0 -ansi -pedantic -g
else
	CFLAGS = -Wall -Wextra -O3 -ansi -pedantic -DNDEBUG
endif

ifeq ($(CACHE), lstatic)
	CACHE_LNK = shm
	CACHE_LINK = -Lcache/$(CACHE_LNK) -lcache$(CACHE_LNK)
	CACHE_OBJS =
else
	CACHE_OBJS = cache/dlloader/cache_loader.o
	DYNAMIC_LINKAGE := $(DYNAMIC_LINKAGE) -DDYNAMIC_LOGGER
	DYNAMIC = -DDYNAMIC
	LIBS := $(LIBS) -ldl
endif

ifeq ($(LOGGER), lstatic)
	LOGGER_LNK = std
	LOGGER_LINK = -Llogger/$(LOGGER_LNK) -llogger$(LOGGER_LNK)
	LOGGER_OBJS = 
else
	LOGGER_OBJS = logger/dlloader/logger_loader.o
	DYNAMIC_LINKAGE := $(DYNAMIC_LINKAGE) -DDYNAMIC_CACHE
	DYNAMIC = -DDYNAMIC
	LIBS := $(LIBS) -ldl
endif


#--------------------------
OBJS=compression.o daemon.o date.o fileutils.o fparams.o main.o \
	request.o response.o socket.o mime.o cgi.o filter.o header_w_quality.o \
	$(CACHE_OBJS) $(LOGGER_OBJS)

all: caches loggers mojito

loggers:
	@(cd logger && make)

caches:
	@(cd cache && make)

mojito: $(OBJS)
	$(LD) $(OBJS) -o $(PNAME) $(LDFLAGS) $(LIBS) $(CACHE_LINK) $(LOGGER_LINK)

.c.o:
	$(GCC) $(INCDIR) -c $< -o $@ $(CFLAGS) $(DYNAMIC) $(DYNAMIC_LINKAGE)

clean:
	-$(RM) $(OBJS)
	-$(RM) $(PNAME)
	@(cd logger && make clean)
	@(cd cache && make clean)

