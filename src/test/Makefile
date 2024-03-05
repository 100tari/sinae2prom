CC=gcc

SRC=test.c ../i2c.c
OBJ=$(SRC:.c=.o)

all: test

test: $(OBJ)
	$(CC) $(OBJ) -o $@ 

clean:
	rm -rf $(OBJ)