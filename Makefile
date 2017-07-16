TARGET=libdict_parser.a
SRCS=dict_parser.cpp builtin_handler.cpp
OBJS=$(SRCS:.cpp=.o)
PUBLIC=./output/

CC=g++
CFLAGS=-fPIC -Wall -finline-functions -pipe -g -rdynamic -Wno-invalid-offsetof -Werror

.PHONY: all clean output

all: $(TARGET) output

output:
	rm -rf output
	mkdir -p output/
	cp dict_parser.h ./output/
	mv *.a ./output/

$(TARGET): $(OBJS)
	rm -f $@
	ar cr $@ $(OBJS)

%.o: %.cpp %.h
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f *.o
	rm -f *.a
