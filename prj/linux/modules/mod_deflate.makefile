.SUFFIXES: .ot .oh

include ../Makefile.config
include ../Makefile.vars

SH_CFLAGS = -fPIC
ST_CFLAGS =

INCLUDE = -I.
SRCDIR = ../../../modules/mod_deflate
OBJDIR = .
BINDIR = ..

all: mod_deflate

sh_pname = libmod_deflate.so.1
st_pname = libmod_deflate.a
sh_objs = mod_deflate.oh
st_objs = mod_deflate.ot

mod_deflate: $(st_objs) $(sh_objs)
	$(GCC) -shared -Wl,-soname,$(sh_pname) -o $(BINDIR)/$(sh_pname) $(sh_objs) -lmmp_base -lc -lz
	$(AR) $(BINDIR)/$(st_pname) $(st_objs)

%.ot: $(SRCDIR)/%.c
	$(GCC) $(INCLUDE) $(CFLAGS) $(SH_CFLAGS) -c $< -o $(OBJDIR)/$@ -DMODULE_STATIC

%.oh: $(SRCDIR)/%.c
	$(GCC) $(INCLUDE) $(CFLAGS) $(ST_CFLAGS) -c $< -o $(OBJDIR)/$@

clean:
	-$(RM) *.oh *.ot
	-$(RM) $(BINDIR)/$(st_pname) $(BINDIR)/$(sh_pname)
