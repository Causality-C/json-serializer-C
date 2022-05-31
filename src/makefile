all: serializer
	gcc -g -o main serializer.o main.c
serializer: serializer.h serializer.c
	gcc -g -c -o serializer.o serializer.c 
clean:
	rm *.o main _*