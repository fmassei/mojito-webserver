.SUFFIXES: .ot .oh

GCC = gcc
AR = ar rcs
RM = /bin/rm -rf

PSTATUS = debug
ifeq ($(PSTATUS), debug)
    SHARED_CFLAGS = -Wall -Wextra -O0 -ansi -pedantic -g -fPIC
    STATIC_CFLAGS = -Wall -Wextra -O0 -ansi -pedantic -g
else
    SHARED_CFLAGS = -Wall -Wextra -O3 -ansi -pedantic -fPIC -DNDEBUG
    STATIC_CFLAGS = -Wall -Wextra -O3 -ansi -pedantic -DNDEBUG
endif

SHARED_PNAME = libcacheshm.so.1
STATIC_PNAME = libcacheshm.a

#-----------------------------------
SHARED_OBJS = linear_hashtab.oh cache.oh
STATIC_OBJS = linear_hashtab.ot cache.ot

all: clean lib

lib: $(STATIC_OBJS) $(SHARED_OBJS)
	$(GCC) -shared -Wl,-soname,$(SHARED_PNAME) -o $(SHARED_PNAME) $(SHARED_OBJS) -lc
	$(AR) $(STATIC_PNAME) $(STATIC_OBJS)

.c.oh:
	$(GCC) -c $< -o $@ $(SHARED_CFLAGS)

.c.ot:
	$(GCC) -c $< -o $@ $(STATIC_CFLAGS) -DMODULE_STATIC

clean:
	-$(RM) *.oh
	-$(RM) *.ot
	-$(RM) $(SHARED_PNAME)
	-$(RM) $(STATIC_PNAME)
