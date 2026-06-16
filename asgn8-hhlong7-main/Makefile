CC = gcc
CFLAGS = -Wall -Wextra -std=gnu11 -pedantic

all: gossip

debug: CFLAGS += -g
debug: gossip

gossip: gossip.c
	$(CC) $(CFLAGS) -o gossip gossip.c

test: gossip
	bash test1.sh && diff tmp1a.txt out1a.txt && diff tmp1b.txt out1b.txt
	bash test2.sh && diff tmp2a.txt out2a.txt && diff tmp2b.txt out2b.txt
	bash test3.sh && diff tmp3a.txt out3a.txt && diff tmp3b.txt out3b.txt && diff tmp3c.txt out3c.txt && diff tmp3d.txt out3d.txt
	bash test4.sh && diff tmp4a.txt out4a.txt && diff tmp4b.txt out4b.txt && diff tmp4c.txt out4c.txt && diff tmp4d.txt out4d.txt && diff tmp4e.txt out4e.txt
	bash test5.sh; diff tmp5.txt err5.txt
	bash test6.sh; diff tmp6.txt err6.txt
	bash test7.sh; diff tmp7.txt err7.txt
	bash test8.sh; diff tmp8.txt err8.txt

clean:
	rm -f gossip tmp*.txt