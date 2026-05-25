freq: dict.c freq.c
	gcc -Wall -Wextra -Werror -ansi -pedantic -o freq dict.c freq.c

debug: dict.c freq.c
	gcc -Wall -Wextra -Werror -ansi -pedantic -g -o freq dict.c freq.c

test: dict.c dicttests.c
	gcc -Wall -Wextra -Werror -ansi -pedantic -o test.out dict.c dicttests.c
	./test.out

valgrind: dict.c dicttests.c
	gcc -Wall -Wextra -Werror -ansi -pedantic -g -o test.out dict.c dicttests.c
	valgrind --leak-check=full --show-leak-kinds=all ./test.out

valgrind-freq: dict.c freq.c
	gcc -Wall -Wextra -Werror -ansi -pedantic -g -o freq dict.c freq.c
	valgrind --leak-check=full --show-leak-kinds=all ./freq in1.txt
