AR=ar
CC=cc
LINK=cc
RANLIB=ranlib
CFLAGS=
CPPFLAGS=

OBJS=vasd.o posix.o vascpp.o
all: vas_test.exe libvas.a

vasd.o: vasd.cpp vasd.h win32.h
	$(CC) -c $(CFLAGS) $(CPPFLAGS) vasd.cpp
	
posix.o: posix.cpp posix.h
	$(CC) -c $(CFLAGS) $(CPPFLAGS) posix.cpp
	
vas_test.o: vas_test.cpp win32.h vasd.h
	$(CC) -c $(CFLAGS) $(CPPFLAGS) vas_test.cpp

libvas.a: $(OBJS)
	rm -f $@
	$(AR) rc $@ $(OBJS)
	$(RANLIB) $@
	
vas_test.exe: libvas.a vas_test.o
	$(LINK) -o vas_test vas_test.o libvas.a

clean:
	rm -f vas_test.exe $(OBJS) libvas.a vas_test.o
