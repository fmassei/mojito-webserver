.SUFFIXES: .ot .oh

include ../Makefile.config
include ../Makefile.vars

SH_CFLAGS = 
ST_CFLAGS = 

INCLUDE = -I.
SRCDIR = ../../../modules/mod_identity
OBJDIR = .
BINDIR = ..

all: mod_identity

sh_pname = libmod_identity.1.dll
st_pname = libmod_identity.a
sh_objs = mod_identity.oh
st_objs = mod_identity.ot

mod_identity: $(st_objs) $(sh_objs)
	$(GCC) -shared -Wl,-soname,$(sh_pname) -o $(BINDIR)/$(sh_pname) $(sh_objs) -lmmp_base -lc
	$(AR) $(BINDIR)/$(st_pname) $(st_objs)

%.ot: $(SRCDIR)/%.c
	$(GCC) $(INCLUDE) $(CFLAGS) $(SH_CFLAGS) -c $< -o $(OBJDIR)/$@ -DMODULE_STATIC

%.oh: $(SRCDIR)/%.c
	$(GCC) $(INCLUDE) $(CFLAGS) $(ST_CFLAGS) -c $< -o $(OBJDIR)/$@

clean:
	-$(RM) *.oh *.ot
	-$(RM) $(BINDIR)/$(st_pname) $(BINDIR)/$(sh_pname)
