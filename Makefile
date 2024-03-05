CC=gcc
MAIN_SRC=src/rpc.c src/i2c.c src/main.c 
MAIN_OBJ=$(MAIN_SRC:.c=.o)
CLI_SRC=src/cli.c src/rpc.c src/i2c.c
CLI_OBJ=$(CLI_SRC:.c=.o)
CLI_FLAGS=-lcli

all: main cli

main: $(MAIN_OBJ)
	$(CC) $(MAIN_OBJ) -o main 

cli: $(CLI_OBJ)
	$(CC) $(CLI_OBJ) -o cli $(CLI_FLAGS)

clean:
	rm -rf $(MAIN_OBJ) $(CLI_OBJ)