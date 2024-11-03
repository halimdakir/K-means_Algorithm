# Target kmeans: to build executable kmeans file from the object files main.o & kmeans.o 
# -g option allow compilation with debugging; -o option tells the output file name 'kmeans'; -lm option links the math library to the program
kmeans: main.o kmeans.o
	gcc -g -o kmeans main.o kmeans.o -lm
	
#Compile C Source file main.c to the associated object file main.o
main.o: main.c
	gcc -g -c main.c
	
#Compile C Source file kmeans.c to the associated object file kmeans.o
kmeans.o: kmeans.c
	gcc -g -c kmeans.c
	
# Valgrind target for memory leak detection (with thorough memory check [leack-check=full] and  track the origins of uninitialized memory [track-origin=yes]
valgrind: kmeans
	valgrind --leak-check=full --track-origins=yes ./kmeans
	
# Target to run the program in debugging mode with GDB
gdb: kmeans
	gdb ./kmeans
	
# Clean up object files and executables; to avoid manual deletion of those files
clean:
	rm -f *.o kmeans
