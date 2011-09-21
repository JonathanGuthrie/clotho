#
# Copyright 2010 Jonathan R. Guthrie
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

CC=g++
CXXFLAGS=-g -Wall -fPIC

%.d: %.cpp
	@set -e; rm -f $@; \
	$(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : Makefile ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

LDFLAGS=-lpthread -lcrypt -shared -Wl,-soname,libclotho.so.1

SOURCES=internetsession.cpp \
	datasource.cpp \
	socket.cpp \
	internetserver.cpp \
	deltaqueue.cpp \
	deltaqueueaction.cpp \
	sessiondriver.cpp \
	servermaster.cpp \
	mutex.cpp \
	cond.cpp \
	thread.cpp \
	server.cpp \
	testserver.cpp

libclotho.so.1.1: internetsession.o socket.o internetserver.o deltaqueue.o deltaqueueaction.o sessiondriver.o servermaster.o mutex.o cond.o thread.o testserver.o server.o
	$(CC) $(LDFLAGS) -o $@ $^

include $(SOURCES:.cpp=.d)

clean:
	rm -f *.o *.d libclotho.so.1.1

