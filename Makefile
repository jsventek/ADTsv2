CFLAGS=-W -Wall -g
REGULAR=altest bqtest hmtest lltest ostest sttest uqtest ustest
THREADSAFE=tsaltest tsbqtest tshmtest tslltest tsostest tssttest tsuqtest tsustest

all: regular threadsafe

regular: $(REGULAR)

threadsafe: $(THREADSAFE)

#
# dependencies for regular test programs
#
altest: altest.o arraylist.o iterator.o
	gcc $(CFLAGS) -o altest $^

bqtest: bqtest.o bqueue.o iterator.o
	gcc $(CFLAGS) -o bqtest $^

hmtest: hmtest.o hashmap.o iterator.o
	gcc $(CFLAGS) -o hmtest $^

lltest: lltest.o linkedlist.o iterator.o
	gcc $(CFLAGS) -o lltest $^

ostest: ostest.o orderedset.o iterator.o
	gcc $(CFLAGS) -o ostest $^

sttest: sttest.o stack.o iterator.o
	gcc $(CFLAGS) -o sttest $^

uqtest: uqtest.o uqueue.o linkedlist.o iterator.o
	gcc $(CFLAGS) -o uqtest $^

ustest: ustest.o unorderedset.o iterator.o
	gcc $(CFLAGS) -o ustest $^

#
# dependencies for thread-safe programs
#
tsaltest: tsaltest.o tsarraylist.o tsiterator.o arraylist.o iterator.o
	gcc $(CFLAGS) -o tsaltest $^ -lpthread

tsbqtest: tsbqtest.o tsbqueue.o tsiterator.o bqueue.o iterator.o
	gcc $(CFLAGS) -o tsbqtest $^ -lpthread

tshmtest: tshmtest.o tshashmap.o tsiterator.o hashmap.o iterator.o
	gcc $(CFLAGS) -o tshmtest $^ -lpthread

tslltest: tslltest.o tslinkedlist.o tsiterator.o linkedlist.o iterator.o
	gcc $(CFLAGS) -o tslltest $^ -lpthread

tsostest: tsostest.o tsorderedset.o tsiterator.o orderedset.o iterator.o
	gcc $(CFLAGS) -o tsostest $^ -lpthread

tssttest: tssttest.o tsstack.o tsiterator.o stack.o iterator.o
	gcc $(CFLAGS) -o tssttest $^ -lpthread

tsuqtest: tsuqtest.o tsuqueue.o tsiterator.o uqueue.o iterator.o linkedlist.o
	gcc $(CFLAGS) -o tsuqtest $^ -lpthread

tsustest: tsustest.o tsunorderedset.o tsiterator.o unorderedset.o iterator.o
	gcc $(CFLAGS) -o tsustest $^ -lpthread

#
# clean up after ourselves
#
clean:
	rm -f $(REGULAR) $(THREADSAFE) *.o

#
# regular object dependencies
#
altest.o: altest.c arraylist.h iterator.h
arraylist.o: arraylist.c arraylist.h iterator.h
bqtest.o: bqtest.c bqueue.h iterator.h
bqueue.o: bqueue.c bqueue.h iterator.h
hmtest.o: hmtest.c hashmap.h iterator.h
hashmap.o: hashmap.c hashmap.h iterator.h
lltest.o: lltest.c linkedlist.h iterator.h
linkedlist.o: linkedlist.c linkedlist.h
ostest.o: ostest.c orderedset.h iterator.h
orderedset.o: orderedset.c orderedset.h iterator.h
sttest.o: sttest.c stack.h iterator.h
stack.o: stack.c stack.h iterator.h
uqtest.o: uqtest.c uqueue.h iterator.h
uqueue.o: uqueue.c uqueue.h linkedlist.h iterator.h
ustest.o: ustest.c unorderedset.h iterator.h
unorderedset.o: unorderedset.c unorderedset.h iterator.h
iterator.o: iterator.c iterator.h
#
# thread-safe object dependencies
#
tsaltest.o: tsaltest.c tsarraylist.h tsiterator.h iterator.h
tsarraylist.o: tsarraylist.c tsarraylist.h tsiterator.h arraylist.h iterator.h
tsbqtest.o: tsbqtest.c tsbqueue.h tsiterator.h iterator.h
tsbqueue.o: tsbqueue.c tsbqueue.h tsiterator.h bqueue.h iterator.h
tshmtest.o: tshmtest.c tshashmap.h tsiterator.h iterator.h
tshashmap.o: tshashmap.c tshashmap.h tsiterator.h hashmap.h iterator.h
tslltest.o: tslltest.c tslinkedlist.h tsiterator.h iterator.h
tslinkedlist.o: tslinkedlist.c tslinkedlist.h tsiterator.h linkedlist.h iterator.h
tsostest.o: tsostest.c tsorderedset.h tsiterator.h iterator.h
tsorderedset.o: tsorderedset.c tsorderedset.h tsiterator.h linkedlist.h iterator.h
tssttest.o: tssttest.c tsstack.h tsiterator.h iterator.h
tsstack.o: tsstack.c tsstack.h tsiterator.h stack.h iterator.h
tsuqtest.o: tsuqtest.c tsuqueue.h tsiterator.h iterator.h
tsqueue.o: tsqueue.c tsuqueue.h tsiterator.h uqueue.h iterator.h
tsustest.o: tsustest.c tsunorderedset.h tsiterator.h iterator.h
tsunorderedset.o: tsunorderedset.c tsunorderedset.h tsiterator.h unorderedset.h iterator.h
tsiterator.o: tsiterator.c tsiterator.h
