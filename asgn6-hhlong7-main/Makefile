batch: batch.c
	gcc -Wall -Wextra -std=gnu11 -pedantic -o batch batch.c

debug: batch.c
	gcc -Wall -Wextra -std=gnu11 -pedantic -g -o batch batch.c

test: batch.c
	gcc -Wall -Wextra -std=gnu11 -pedantic -o batch batch.c
	gcc -o testprog testprog.c
	./batch $(shell cat arg1.txt) | diff - out1.txt
	./batch $(shell cat arg2.txt) 2>&1 | diff - both2.txt
	./batch $(shell cat arg3.txt) | diff - out3.txt
	./batch $(shell cat arg4.txt) 2>&1 | diff - both4.txt
	./batch $(shell cat arg5.txt) 2>&1 | diff - err5.txt
	./batch $(shell cat arg6.txt) 2>&1 | diff - err6.txt
	./batch $(shell cat arg7.txt) 2>&1 | diff - err7.txt
	./batch $(shell cat arg8.txt) 2>&1 | diff - err8.txt

valgrind: batch.c
	gcc -Wall -Wextra -std=gnu11 -pedantic -g -o batch batch.c
	gcc -o testprog testprog.c
	valgrind --leak-check=full --show-leak-kinds=all ./batch $(shell cat arg1.txt)
	valgrind --leak-check=full --show-leak-kinds=all ./batch $(shell cat arg2.txt)
	valgrind --leak-check=full --show-leak-kinds=all ./batch $(shell cat arg3.txt)
	valgrind --leak-check=full --show-leak-kinds=all ./batch $(shell cat arg4.txt)
