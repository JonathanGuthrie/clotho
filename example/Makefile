CC=g++
CXXFLAGS=-g -Wall -I..

%.d: %.cpp
	@set -e; rm -f $@; \
	$(CC) -MM $(CXXFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

LDFLAGS=-lpthread -lcrypt -lcppserver

SOURCES=echod.cpp \
	echomaster.cpp \
	echosession.cpp \
	echorequest.cpp \
	idletimer.cpp

echod: echod.o echomaster.o echosession.o echorequest.o idletimer.o

include $(SOURCES:.cpp=.d)

echod.o:  Makefile

echomaster.o: Makefile

echosession.o: Makefile

echorequest.o: Makefile

idletimer.o: Makefile

clean:
	rm -f *.o *.d echod

