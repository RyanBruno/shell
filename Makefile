CC = cc
FLAGS = -g -Wall -Werror

ALL: main.o icmd.o
	$(CC) $(FLAGS) -o main main.o icmd.o

main.o: main.c
	$(CC) -c $(FLAGS) main.c

icmd.o: commandHandler.c commandHandler.h
	$(CC) -c $(FLAGS) commandHandler.c

clean:
	rm *.o
	rm main
