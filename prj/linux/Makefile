include Makefile.config
include Makefile.vars

GCCFLAG = $(CFLAGS)
LDFLAG = -ldisml -lpthread -ldl -lmmp_base -lrt -Wl,-E
INCLUDE = -I.
SRCDIR = ../..
OBJDIR = .
BINDIR = .
FINAME = mojito

OBJS = main.o config_parse.o config_manager.o socket_unit.o \
		socket_unit_manager.o request.o request_parse.o header_w_quality.o \
		modules.o mime.o response.o resp_headers.o filter_manager.o \
        module_loader.o
OBJWD = echo $(OBJDIR)/$(OBJS) | sed 's/\ /\ \.\//g'

base: all modules_all
clean: main_clean modules_clean

%.o: $(SRCDIR)/%.c
	$(GCC) $(INCLUDE) $(GCCFLAG) -c $< -o $(OBJDIR)/$@

main_clean:
	-$(RM) $(OBJDIR)/*.o
	-$(RM) $(BINDIR)/$(FINAME)

all: $(OBJS)
	$(LD) $(shell $(OBJWD)) $(LDFLAG) -o $(BINDIR)/$(FINAME) 

modules_all:
	@(cd modules; make -f Makefile.modules)

modules_clean:
	@(cd modules; make -f Makefile.modules modules_clean)

