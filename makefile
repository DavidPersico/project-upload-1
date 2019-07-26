CC = gcc $(CFLAGS)
OBJ = error.o response_handler.o interrupt_handler.o concat.o file_info.o
LIB = -lpthread
CFLAGS = -g

all: web_server clean

web_server: $(OBJ) simple_web_server.c
		$(CC) $(CFLAGS) -o $@ $^ $(LIB)

%.o: %.c %.h
		$(CC) $(CFLAGS) -c $<

clean:
		rm $(OBJ)
