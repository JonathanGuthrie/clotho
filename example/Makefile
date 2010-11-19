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
CXXFLAGS=-g -Wall -I..

%.d: %.cpp
	@set -e; rm -f $@; \
	$(CC) -MM $(CXXFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

LDFLAGS=-lpthread -lcrypt -lclotho

SOURCES=echod.cpp \
	echomaster.cpp \
	echosession.cpp \
	idletimer.cpp

echod: echod.o echomaster.o echosession.o idletimer.o

include $(SOURCES:.cpp=.d)

echod.o:  Makefile

echomaster.o: Makefile

echosession.o: Makefile

idletimer.o: Makefile

clean:
	rm -f *.o *.d echod

