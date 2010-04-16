CC=g++
CXXFLAGS=-g -Wall -Iserverlib

%.d: %.cpp
	@set -e; rm -f $@; \
	$(CC) -MM $(CXXFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

LOCALLIBS=serverlib/libserverlib.so

LDFLAGS=-lpthread -lcrypt -Lserverlib -lserverlib

SOURCES=httpd.cpp \
	httpmaster.cpp \
	httpfactory.cpp \
	httpsession.cpp \
	httpdriver.cpp

httpd: httpd.o httpmaster.o httpfactory.o httpsession.o httpdriver.o $(LOCALLIBS)

include $(SOURCES:.cpp=.d)

httpd.o:  Makefile

httpmaster.o: Makefile

httpfactory.o: Makefile

httpsession.o: Makefile

httpdriver.o: Makefile

serverlib/libserverlib.so:
	make -C serverlib libserverlib.so

clean:
	rm -f *.o *.d httpd
	make -C serverlib clean

