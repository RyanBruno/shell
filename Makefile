CC = cc
FLAGS = -g -Wall -Werror

ALL: main.o commandHandler.o accnt.o
	$(CC) $(FLAGS) -o main main.o commandHandler.o accnt.o

main.o: main.c
	$(CC) -c $(FLAGS) main.c

commandHandler.o: commandHandler.c commandHandler.h
	$(CC) -c $(FLAGS) commandHandler.c

accnt.o: accnt.c accnt.h
	$(CC) -c $(FLAGS) accnt.c

clean:
	rm *.o
	rm main
