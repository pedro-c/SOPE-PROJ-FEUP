all: rmdup.c listdir.c 
	gcc -Wall rmdup.c -o ./bin/sw
	gcc -Wall listdir.c -o ./bin/csc
	
