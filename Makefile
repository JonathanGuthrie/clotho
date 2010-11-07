CC=g++
CXXFLAGS=-g -Wall -fPIC

%.d: %.cpp
	@set -e; rm -f $@; \
	$(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : Makefile ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

LDFLAGS=-lpthread -lcrypt -shared -Wl,-soname,libclotho.so.1

SOURCES=internetsession.cpp \
	socket.cpp \
	internetserver.cpp \
	deltaqueue.cpp \
	deltaqueueaction.cpp \
	sessiondriver.cpp \
	servermaster.cpp \
	mutex.cpp \
	cond.cpp \
	thread.cpp

libclotho.so.1.1: internetsession.o socket.o internetserver.o deltaqueue.o deltaqueueaction.o sessiondriver.o servermaster.o mutex.o cond.o thread.o
	$(CC) $(LDFLAGS) -o $@ $^

include $(SOURCES:.cpp=.d)

clean:
	rm -f *.o *.d libclotho.so.1.1

