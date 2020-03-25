CC = cc
FLAGS = -g -Wall -Werror

ALL: main.o commandHandler.o
	$(CC) $(FLAGS) -o main main.o commandHandler.o

main.o: main.c
	$(CC) -c $(FLAGS) main.c

commandHandler.o: commandHandler.c commandHandler.h
	$(CC) -c $(FLAGS) commandHandler.c

clean:
	rm *.o
	rm main
