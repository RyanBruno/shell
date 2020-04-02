CC = cc
FLAGS = -g -Wall -Werror

ALL: main.o
	$(CC) $(FLAGS) -o main main.o

main.o: main.c commandHandler.c accounting.c
	$(CC) -c $(FLAGS) main.c


clean:
	rm *.o
	rm main
