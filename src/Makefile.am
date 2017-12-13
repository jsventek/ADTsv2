##########################################################################################
AUTOMAKE_OPTIONS = foreign subdir-objects
ACLOCAL_FLAGS = -I m4

lib_LTLIBRARIES = libADTs.la
TESTS = ./test.sh

srpcincludedir = $(includedir)/adts
srpcinclude_HEADERS = arraylist.h bqueue.h hashmap.h iterator.h linkedlist.h orderedset.h stack.h unorderedset.h uqueue.h tsarraylist.h tsbqueue.h tshashmap.h tsiterator.h tslinkedlist.h tsorderedset.h tsstack.h tsunorderedset.h tsuqueue.h

libADTs_la_SOURCES = iterator.c arraylist.c bqueue.c hashmap.c linkedlist.c orderedset.c stack.c unorderedset.c uqueue.c tsiterator.c tsarraylist.c tsbqueue.c tshashmap.c tslinkedlist.c tsorderedset.c tsstack.c tsunorderedset.c tsuqueue.c

