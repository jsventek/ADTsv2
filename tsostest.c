/*
 * Copyright (c) 2017, University of Oregon
 * All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:

 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * - Neither the name of the University of Oregon nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "tsorderedset.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int scmp(void *a, void *b) {
    return strcmp((char *)a, (char *)b);
}

int main(int argc, char *argv[]) {
    char buf[1024];
    char *p;
    const TSOrderedSet *ts;
    long i, n;
    FILE *fd;
    const TSIterator *it;
    void **array;

    if (argc != 2) {
        fprintf(stderr, "usage: ./tstest file\n");
        return -1;
    }
    if ((ts = TSOrderedSet_create(scmp)) == NULL) {
        fprintf(stderr, "Error creating treeset of strings\n");
        return -1;
    }
    if ((fd = fopen(argv[1], "r")) == NULL) {
        fprintf(stderr, "Unable to open %s to read\n", argv[1]);
        return -1;
    }
    /*
     * test of add()
     */
    printf("===== test of add\n");
    i = 0;
    while (fgets(buf, 1024, fd) != NULL) {
        p = strchr(buf, '\n');
        *p = '\0';
        if ((p = strdup(buf)) == NULL) {
            fprintf(stderr, "Error duplicating string\n");
            return -1;
        }
        if (!ts->add(ts, p)) {
            fprintf(stderr, "Duplicate line: \"%s\"\n", p);
            free(p);
        }
    }
    fclose(fd);
    n = ts->size(ts);
    /*
     * test of get()
     */
    printf("===== test of first and remove\n");
    printf("Size before remove = %ld\n", n);
    for (i = 0; i < n; i++) {
        char *element;

        if (!ts->first(ts, (void **)&element)) {
            fprintf(stderr, "Error retrieving %ld'th element\n", i);
            return -1;
        }
        printf("%s\n", element);
        if (!ts->remove(ts, element, free)) {
            fprintf(stderr, "Error removing %ld'th element\n", i);
            return -1;
        }
    }
    printf("Size after remove = %ld\n", ts->size(ts));
    /*
     * test of destroy with NULL freeFxn
     */
    printf("===== test of destroy(NULL)\n");
    ts->destroy(ts, NULL);
    /*
     * test of insert
     */
    if ((ts = TSOrderedSet_create(scmp)) == NULL) {
        fprintf(stderr, "Error creating treeset of strings\n");
        return -1;
    }
    fd = fopen(argv[1], "r");		/* we know we can open it */
    i = 0L;
    while (fgets(buf, 1024, fd) != NULL) {
        p = strchr(buf, '\n');
        *p = '\0';
        if ((p = strdup(buf)) == NULL) {
            fprintf(stderr, "Error duplicating string\n");
            return -1;
        }
        if (!ts->add(ts, p)) {
            free(p);
        }
    }
    fclose(fd);
    /*
     * test of toArray
     */
    printf("===== test of toArray\n");
    if ((array = ts->toArray(ts, &n)) == NULL) {
        fprintf(stderr, "Error in invoking ts->toArray()\n");
        return -1;
    }
    for (i = 0; i < n; i++) {
        printf("%s\n", (char *)array[i]);
    }
    free(array);
    /*
     * test of iterator
     */
    printf("===== test of iterator\n");
    if ((it = ts->itCreate(ts)) == NULL) {
        fprintf(stderr, "Error in creating iterator\n");
        return -1;
    }
    while (it->hasNext(it)) {
        char *p;
        (void) it->next(it, (void **)&p);
        printf("%s\n", p);
    }
    it->destroy(it);
    /*
     * test of ceiling, floor, higher, lower
     */
    if (!ts->ceiling(ts, "0005", (void **)&p)) {
        fprintf(stderr, "No ceiling found relative to \"0005\"\n");
    } else
        printf("Ceiling relative to \"0005\" is \"%s\"\n", p);
    if (!ts->higher(ts, "0006", (void **)&p)) {
        fprintf(stderr, "No higher found relative to \"0006\"\n");
    } else
        printf("Higher relative to \"0006\" is \"%s\"\n", p);
    if (!ts->floor(ts, "0005", (void **)&p)) {
        fprintf(stderr, "No floor found relative to \"0005\"\n");
    } else
        printf("Floor relative to \"0005\" is \"%s\"\n", p);
    if (!ts->lower(ts, "0006", (void **)&p)) {
        fprintf(stderr, "No lower found relative to \"0006\"\n");
    } else
        printf("Lower relative to \"0006\" is \"%s\"\n", p);
    /*
     * test of pollFirst and pollLast
     */
    n = ts->size(ts) / 4;
    printf("===== test of pollFirst - first %ld elements of the set are\n", n);
    for (i = 0; i < n; i++) {
        char *p;
        (void) ts->first(ts, (void **)&p);
        printf("First element is: \"%s\"\n", p);
        (void) ts->last(ts, (void **)&p);
        printf("Last element is: \"%s\"\n", p);
        if (!ts->pollFirst(ts, (void **)&p)) {
            fprintf(stderr, "Error invoking pollFirst()\n");
            return -1;
        }
        printf("%s\n", p);
        free(p);
    }
    printf("===== test of pollLast - last %ld elements of the set are\n", n);
    for (i = 0; i < n; i++) {
        char *p;
        (void) ts->first(ts, (void **)&p);
        printf("First element is: \"%s\"\n", p);
        (void) ts->last(ts, (void **)&p);
        printf("Last element is: \"%s\"\n", p);
        if (!ts->pollLast(ts, (void **)&p)) {
            fprintf(stderr, "Error invoking pollLast()\n");
            return -1;
        }
        printf("%s\n", p);
        free(p);
    }
    /*
     * test of destroy with free() as freeFxn
     */
    printf("===== test of destroy(free)\n");
    ts->destroy(ts, free);

    return 0;
}
