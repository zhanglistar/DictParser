CC=g++
CFLAGS=-g -Werror

INCLUDEDIR=-I../output/

LIBDIR=-L../output/ \
	   -lparser

demo: main.cpp comp_data_t_handler.cpp
	$(CC) $(CFLAGS) -o $@ $^ $(INCLUDEDIR) $(LIBDIR)

.PHONY: clean

clean:
	rm -f demo
