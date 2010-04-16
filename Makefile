CC=g++
CXXFLAGS=-g -Wall -fPIC

%.d: %.cpp
	@set -e; rm -f $@; \
	$(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

LDFLAGS=-lpthread -lcrypt -shared -Wl,-soname,libserverlib.so.1

SOURCES=internetsession.cpp \
	socket.cpp \
	internetserver.cpp \
	deltaqueue.cpp \
	deltaqueueaction.cpp \
	sessiondriver.cpp \
	sessionfactory.cpp \
	servermaster.cpp

libserverlib.so.1.1: internetsession.o socket.o internetserver.o deltaqueue.o deltaqueueaction.o sessiondriver.o sessionfactory.o servermaster.o
	$(CC) $(LDFLAGS) -o $@ $^

include $(SOURCES:.cpp=.d)

internetsession.o: Makefile

socket.o: Makefile

internetserver.o: Makefile

deltaqueue.o: Makefile

deltaqueueaction.o: Makefile

sessiondriver.o: Makefile

sessionfactory.o: Makefile

servermaster.o: Makefile

clean:
	rm -f *.o *.d libserverlib.so.1.1

