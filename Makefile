PATHS = ${PWD}
CC = g++
INCLUDES = $(addprefix -I, $(addsuffix /include, $(PATHS)))
CFLAGS = -std=c++14 -g $(INCLUDES)
LFLAGS = $(addprefix -L, $(addsuffix /lib, $(PATHS)))
WFLAGS = -Wfatal-errors
SOURCES = $(notdir $(wildcard src/*.cpp))
OBJECTS = $(addprefix obj/, $(SOURCES:.cpp=.o))
LIBRARY = event
LIBS = $(addprefix -l, $(LIBRARY))

$(LIBRARY): lib/lib$(LIBRARY).a

lib/lib%.a: $(OBJECTS)
	ar rcs $@ $^

obj/%.o: src/%.cpp include/%.hpp
	$(CC) $(WFLAGS) $(CFLAGS) -c $< -o $@

test: bin/test

bin/%: main.cpp lib/lib$(LIBRARY).a
	$(CC) $(WFLAGS) $(CFLAGS) $(LFLAGS) -o $@ $< $(LIBS) -pthread

clean:
	-rm bin/* lib/lib$(LIBRARY).a $(OBJECTS)
