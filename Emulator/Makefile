SOURCE = functions.cpp \
         main.cpp

#GNU C/C++ Compiler
GCC = g++

# GNU C/C++ Linker
LINK = g++

# Compiler flags
CFLAGS = -Wall -O3 -std=c++11
CXXFLAGS = $(CFLAGS) 

.PHONY: clean

# Targets include all, clean, debug, tar

all : main

main: $(SOURCE)
	$(LINK) -o $@ $^ $(LIBS)

clean:
	rm -rf *.o *.d core main

debug: CXXFLAGS = -DDEBUG -g -std=c++11
debug: main

tar: clean
	tar zcvf prog3.tgz $(SOURCE) *.h Makefile

%.d: %.cpp
	@set -e; /bin/rm -rf $@;$(GCC) -MM $< $(CXXFLAGS) > $@
