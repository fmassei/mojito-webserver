.SUFFIXES: .ot .oh

include ../Makefile.config
include ../Makefile.vars

SH_CFLAGS = -fPIC
ST_CFLAGS = 

INCLUDE = -I.
SRCDIR = ../../../modules/mod_identity
OBJDIR = .
BINDIR = .

all: mod_identity

sh_pname = libmod_identity.so.1
st_pname = libmod_identity.a
sh_objs = mod_identity.oh
st_objs = mod_identity.ot
mod_identity: $(st_objs) $(sh_objs)
	$(GCC) -shared -Wl,-soname,$(sh_pname) -o $(BINDIR)/$(sh_pname) $(sh_objs) -lmmp_base -lc
	$(AR) $(BINDIR)/$(st_pname) $(st_objs)

%.ot: $(SRCDIR)/%.c
	$(GCC) $(INCLUDE) $(SH_CFLAGS) -c $< -o $(OBJDIR)/$@

%.oh: $(SRCDIR)/%.c
	$(GCC) $(INCLUDE) $(ST_CFLAGS) -c $< -o $(OBJDIR)/$@ -DMODULE_STATIC

clean:
	-$(RM) *.oh *.ot
	-$(RM) $(BINDIR)/$(st_pname) $(BINDIR)/$(sh_pname)
