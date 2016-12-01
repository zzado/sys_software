sic: main.o pass1.o pass2.o
	gcc -o sic main.o pass1.o pass2.o

main.o: main.c
	gcc -c main.c

pass1.o: pass1.c
	gcc -c pass1.c

pass2.o: pass2.c
	gcc -c pass2.c

clean: 
	rm main.o pass1.o pass2.o
