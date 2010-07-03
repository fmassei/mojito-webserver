include Makefile.config
include Makefile.vars

GCCFLAG = $(CFLAGS)
LDFLAG = -L../../deps/lib/linux -ldisml -lpthread
ELIB = ../../deps/lib/linux/libmmp_base.a
INCLUDE = -I../../deps/include -I../../deps/include/mmp \
			-I../../deps/include/disml
SRCDIR = ../..
OBJDIR = .
BINDIR = .
FINAME = mojito

OBJS = main.o config_parse.o config_manager.o socket_unit.o \
		socket_unit_manager.o request.o request_parse.o header_w_quality.o \
		modules.o
OBJWD = echo $(OBJDIR)/$(OBJS) | sed 's/\ /\ \.\//g'

base: all modules_all
clean: main_clean modules_clean

%.o: $(SRCDIR)/%.c
	$(GCC) $(INCLUDE) $(GCCFLAG) -c $< -o $(OBJDIR)/$@

main_clean:
	-$(RM) $(OBJDIR)/*.o
	-$(RM) $(BINDIR)/$(FINAME)

all: $(OBJS)
	$(LD) $(LDFLAG) -o $(BINDIR)/$(FINAME) $(shell $(OBJWD)) $(ELIB)

modules_all:
	@(cd modules; make -f Makefile.modules)

modules_clean:
	@(cd modules; make -f Makefile.modules modules_clean)

