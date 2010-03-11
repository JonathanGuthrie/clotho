CC=g++
CXXFLAGS=-g -Wall
LDFLAGS=-lpthread -lcrypt

%.d: %.cpp
	@set -e; rm -f $@; \
	$(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

SOURCES=httpd.cpp \
	internetsession.cpp \
	socket.cpp \
	internetserver.cpp \
	deltaqueue.cpp \
	deltaqueueaction.cpp \
	sessiondriver.cpp \
	sessionfactory.cpp \
	servermaster.cpp \
	httpmaster.cpp \
	httpfactory.cpp \
	httpsession.cpp \
	httpdriver.cpp

httpd: httpd.o internetserver.o internetsession.o socket.o deltaqueue.o deltaqueueaction.o sessiondriver.o sessionfactory.o servermaster.o httpmaster.o httpfactory.o httpsession.o httpdriver.cpp

include $(SOURCES:.cpp=.d)

httpd.o:  Makefile

internetsession.o: Makefile

socket.o:  Makefile

internetserver.o:  Makefile

deltaqueue.o:  Makefile

deltaqueueaction.o:  Makefile

sessiondriver.o: Makefile

sessionfactory.o: Makefile

servermaster.o: Makefile

httpmaster.o: Makefile

httpfactory.o: Makefile

httpsession.o: Makefile

httpdriver.o: Makefile

clean:
	rm -f *.o *.d httpd

