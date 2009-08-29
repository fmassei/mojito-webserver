.SUFFIXES: .ot .oh

GCC = gcc
AR = ar rcs
SHARED_CFLAGS = -Wall -Wextra -O3 -ansi -pedantic -fPIC
STATIC_CFLAGS = -Wall -Wextra -O3 -ansi -pedantic
RM = /bin/rm -rf

SHARED_PNAME = libloggerstd.so.1
STATIC_PNAME = libloggerstd.a

#-----------------------------------
SHARED_OBJS = logger.oh
STATIC_OBJS = logger.ot

all: lib

lib: $(STATIC_OBJS) $(SHARED_OBJS)
	$(GCC) -shared -Wl,-soname,$(SHARED_PNAME) -o $(SHARED_PNAME) $(SHARED_OBJS) -lc
	$(AR) $(STATIC_PNAME) $(STATIC_OBJS)

.c.oh:
	$(GCC) -c $< -o $@ $(SHARED_CFLAGS)

.c.ot:
	$(GCC) -c $< -o $@ $(STATIC_CFLAGS)

clean:
	-$(RM) *.oh
	-$(RM) *.ot
	-$(RM) $(SHARED_PNAME)
	-$(RM) $(STATIC_PNAME)
