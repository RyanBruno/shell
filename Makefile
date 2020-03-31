CC = cc
FLAGS = -g -Wall -Werror

ALL: main.o accnt.o
	$(CC) $(FLAGS) -o main main.o accnt.o

main.o: main.c commandHandler.c
	$(CC) -c $(FLAGS) main.c

accnt.o: accnt.c accnt.h
	$(CC) -c $(FLAGS) accnt.c

clean:
	rm *.o
	rm main
