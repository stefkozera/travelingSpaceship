# Compiler/linker setup ------------------------------------------------------
# make to make
# ./travelingSpaceship to run

# Mac OS X-specific flags.  Comment these out if using Linux.
PLATFORM = osx
CC       = g++
CFLAGS   = -Wall
OSLIBS   = 
LDFLAGS  =

OBJS = travelingSpaceship
all: $(OBJS)

travelingSpaceship: travelingSpaceship.cpp
	$(CC) $(CFLAGS) travelingSpaceship.cpp $(LDFLAGS) -o travelingSpaceship

# Maintenance and stuff ------------------------------------------------------
clean:
	rm -f $(OBJS) *.o core