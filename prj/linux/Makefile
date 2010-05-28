GCC = gcc
LD = gcc
RM = rm -f
AR = ar rcs
GCCFLAG = -Wall -ansi -pedantic -fPIC
LDFLAG = -L../../deps/lib/linux -ldisml
INCLUDE = -I../../deps/include -I../../deps/include/mmp \
			-I../../deps/include/disml
SRCDIR = ../..
OBJDIR = .
BINDIR = .
FINAME = mojito

OBJS = main.o config_parse.o config_manager.o
OBJWD = echo $(OBJDIR)/$(OBJS) | sed 's/\ /\ \.\//g'

base: all

%.o: $(SRCDIR)/%.c
	$(GCC) $(INCLUDE) $(GCCFLAG) -c $< -o $(OBJDIR)/$@

clean:
	-$(RM) $(OBJDIR)/*.o
	-$(RM) $(BINDIR)/$(FINAME)

all: $(OBJS)
	$(LD) $(LDFLAG) -o $(BINDIR)/$(FINAME) $(shell $(OBJWD))

debug: GCCFLAG += -g
debug: all
