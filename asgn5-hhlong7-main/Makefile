tree: tree.c
	gcc -Wall -Wextra -std=gnu11 -pedantic -o tree tree.c

debug: tree.c
	gcc -Wall -Wextra -std=gnu11 -pedantic -g -o tree tree.c

test: tree.c
	gcc -Wall -Wextra -std=gnu11 -pedantic -o tree tree.c
	./tree testdir | diff - out1.txt
	./tree -d 2 testdir | diff - out2.txt
	./tree -a -l testdir | diff - out3.txt
	./tree testdir/bar.txt | diff - out4.txt
	./tree -b testdir 2>&1 | diff - err5.txt
	./tree -d one testdir 2>&1 | diff - err6.txt
	./tree dne | diff - out7.txt
	./tree testdir/foo.txt/foo.txt | diff - out8.txt

valgrind: tree.c
	gcc -Wall -Wextra -std=gnu11 -pedantic -g -o tree tree.c
	valgrind --leak-check=full --show-leak-kinds=all ./tree testdir
	valgrind --leak-check=full --show-leak-kinds=all ./tree -a -l testdir
	valgrind --leak-check=full --show-leak-kinds=all ./tree -d 2 testdir
