CC=gcc
MAIN_SRC=src/rpc.c src/i2c.c src/main.c 
MAIN_OBJ=$(MAIN_SRC:.c=.o)

main: $(MAIN_OBJ)
	$(CC) $(MAIN_OBJ) -o main 

clean:
	rm -rf $(MAIN_OBJ)