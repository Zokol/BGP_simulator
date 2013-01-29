## An example of a Makefile for linux/cygwin.
#### Define a proper name for your executable (EXE).
#### List the cc/cpp files to be included in the model (SRCS).
#### Define your system: linux/cygwin (T_ARCH).
#### Define your SystemC installation directory (SYSTEMC).


## Name of the executable
EXE    = bgPSim
## cpp files of the model
SRCS     = $(wildcard ./*.cpp)
## header files of the model
SRCH     = $(wildcard ./*.hpp)

## Objects from cpp files
OBJS     = $(SRCS:.cpp=.o)

## Library to be used: linux or cygwin
## for example T_ARCH = cygwin
T_ARCH =linux
## Compiler
CC     = g++
## Debug flag
DEBUG  = -g
## Other options
OPT    =
## SystemC install directory 
##for example, SYSTEMC = /usr/local/lib/systemc-2.3
SYSTEMC = /systemc-2.3.0

## Included source directories
INCDIR = -I $(SYSTEMC)/include
## Included library directories
LIBDIR = -L $(SYSTEMC)/lib-$(T_ARCH)

## Build with maximum gcc warning level
CFLAGS = -Wall $(DEBUG) $(OPT)
## More libraries
LIBS   =    -lsystemc-2.3.0 -Wl,-rpath,$(SYSTEMC)/lib-$(T_ARCH) -lstdc++ -lm

## Define 'all'
all:$(EXE)

## Build
$(EXE): $(OBJS) $(SRCH) $(SYSTEMC)/lib-$(T_ARCH)/libsystemc.a 

	$(CC) $(CFLAGS) $(INCDIR) $(LIBDIR) -o $@ $(OBJS) $(LIBS) 2>&1 | c++filt

.cpp.o:
	$(CC) $(CFLAGS) $(INCDIR) -c $<

## Cleaning if needed
clean:
	rm -f $(OBJS) *~ $(EXE) *.dat *.vcd

ultraclean: clean
	rm -f Makefile.deps

## File dependencies
Makefile.deps: $(SRCS)	
	$(CC) $(CFLAGS) $(INCDIR) -M $(SRCS) >> Makefile.deps
