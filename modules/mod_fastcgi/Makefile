.SUFFIXES: .ot .oh

include ../../Makefile.flags

SHARED_PNAME = libmod_fcgi.so.1
STATIC_PNAME = libmod_fcgi.a

ifeq ($(PSTATUS), debug)
    SHARED_CFLAGS = -Wall -Wextra -O0 -ansi -pedantic -g -fPIC
    STATIC_CFLAGS = -Wall -Wextra -O0 -ansi -pedantic -g
else
    SHARED_CFLAGS = -Wall -Wextra -O3 -ansi -pedantic -fPIC -DNDEBUG
    STATIC_CFLAGS = -Wall -Wextra -O3 -ansi -pedantic -DNDEBUG
endif
#-----------------------------------
SHARED_OBJS = mod_fcgi.oh fcgi.oh linear_array.oh ../modutils.oh
STATIC_OBJS = mod_fcgi.ot fcgi.ot linear_array.ot ../modutils.ot

all: lib

lib: $(STATIC_OBJS) $(SHARED_OBJS)
	$(GCC) -shared -Wl,-soname,$(SHARED_PNAME) -o $(SHARED_PNAME) $(SHARED_OBJS) -lc -lrt
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
