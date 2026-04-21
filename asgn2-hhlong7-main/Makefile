hist: hist.o helper.o
	gcc -Wall -Wextra -Werror -ansi -pedantic hist.o helper.o -o hist

hist.o: hist.c hist.h
	gcc -Wall -Wextra -Werror -ansi -pedantic -c hist.c

helper.o: helper.c hist.h
	gcc -Wall -Wextra -Werror -ansi -pedantic -c helper.c
