lzw: lzw.c dict.c
	gcc -Wall -Wextra -std=gnu11 -pedantic -o lzw lzw.c dict.c

debug: lzw.c dict.c
	gcc -Wall -Wextra -std=gnu11 -pedantic -g -o lzw lzw.c dict.c

valgrind-encode: lzw.c dict.c
	gcc -Wall -Wextra -std=gnu11 -pedantic -g -o lzw lzw.c dict.c
	valgrind --leak-check=full --show-leak-kinds=all ./lzw -c in1.txt tmp.bin
	valgrind --leak-check=full --show-leak-kinds=all ./lzw -c in2.txt tmp.bin
	valgrind --leak-check=full --show-leak-kinds=all ./lzw -c in3.txt tmp.bin
	valgrind --leak-check=full --show-leak-kinds=all ./lzw -c in4.txt tmp.bin

valgrind-decode: lzw.c dict.c
	gcc -Wall -Wextra -std=gnu11 -pedantic -g -o lzw lzw.c dict.c
	valgrind --leak-check=full --show-leak-kinds=all ./lzw -x out1.bin tmp.txt
	valgrind --leak-check=full --show-leak-kinds=all ./lzw -x out2.bin tmp.txt
	valgrind --leak-check=full --show-leak-kinds=all ./lzw -x out3.bin tmp.txt
	valgrind --leak-check=full --show-leak-kinds=all ./lzw -x out4.bin tmp.txt