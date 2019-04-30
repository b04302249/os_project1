scheduler: main.o scheduler.o	
	gcc -o scheduler main.o scheduler.o
main.o: main.c
	gcc -c main.c
scheduler.o: scheduler.h scheduler.c
	gcc -c scheduler.c
time.o: time.c
	gcc -c time.c

ctags:
	ctags scheduler.h scheduler.c main.c

clean:
	rm -f scheduler main.o scheduler.o time.o
	rm -f tags

