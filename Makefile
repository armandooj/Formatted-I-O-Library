CC=gcc

#	my_stdio.c
lib:
	gcc -c my_stdio.c -std=c99 -fPIC;\


#STATIC working

normal: my_stdio.o
	ar q libmy_stdio.a my_stdio.o; \
	gcc -o test_buffered_STATIC test_buffered.c -L. libmy_stdio.a

static_test: test_buffered_STATIC
	./test_buffered_STATIC source.txt dest.txt ;



#Dynamic 1

dynamic:
	gcc -shared -o libmy_stdio.so my_stdio.o;\
	gcc -o test_buffered_DYNAMIC test_buffered.c -L. libmy_stdio.so	

dynamic_test: test_buffered_DYNAMIC
	LD_PRELOAD=./libmy_stdio.so ./test_buffered_DYNAMIC source.txt dest.txt
	LD_PRELOAD=""

dest:
	more dest.txt;

help:
	ldd ./test_buffered_DYNAMIC


clean:
	rm -f *.o *~ *.so *.a test_buffered_STATIC  test_buffered_DYNAMIC  libmy_stdio.so dest.txt;\
	touch dest.txt