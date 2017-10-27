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

#include "tslinkedlist.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    char buf[1024];
    char *p;
    const TSLinkedList *ll;
    long i, n;
    FILE *fd;
    char **array;
    const TSIterator *it;

    if (argc != 2) {
        fprintf(stderr, "usage: ./lltest file\n");
        return -1;
    }
    if ((ll = TSLinkedList_create()) == NULL) {
        fprintf(stderr, "Error creating linked list of strings\n");
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
    while (fgets(buf, 1024, fd) != NULL) {
        if ((p = strdup(buf)) == NULL) {
            fprintf(stderr, "Error duplicating string\n");
            return -1;
        }
        if (!ll->add(ll, p)) {
            fprintf(stderr, "Error adding string to linked list\n");
            return -1;
        }
    }
    fclose(fd);
    n = ll->size(ll);
    /*
     * test of get()
     */
    printf("===== test of get\n");
    for (i = 0; i < n; i++) {
        if (!ll->get(ll, i, (void **)&p)) {
            fprintf(stderr, "Error retrieving %ld'th element\n", i);
            return -1;
        }
        printf("%s", p);
    }
    /*
     * test of remove
     */
    printf("===== test of remove\n");
    for (i = n - 1; i >= 0; i--) {
        if (!ll->remove(ll, i, (void **)&p)) {
            fprintf(stderr, "Error removing string from linked list\n");
            return -1;
        }
        free(p);
    }
    /*
     * test of destroy with NULL freeFxn
     */
    printf("===== test of destroy(NULL)\n");
    ll->destroy(ll, NULL);
    /*
     * test of insert
     */
    if ((ll = TSLinkedList_create()) == NULL) {
        fprintf(stderr, "Error creating linked list of strings\n");
        return -1;
    }
    fd = fopen(argv[1], "r");		/* we know we can open it */
    printf("===== test of insert\n");
    while (fgets(buf, 1024, fd) != NULL) {
        if ((p = strdup(buf)) == NULL) {
            fprintf(stderr, "Error duplicating string\n");
            return -1;
        }
        if (!ll->insert(ll, 0, p)) {
            fprintf(stderr, "Error adding string to linked list\n");
            return -1;
        }
    }
    fclose(fd);
    for (i = 0; i < n; i++) {
        if (!ll->get(ll, i, (void **)&p)) {
            fprintf(stderr, "Error retrieving %ld'th element\n", i);
            return -1;
        }
        printf("%s", p);
    }
    /*
     * test of set
     */
    printf("===== test of set\n");
    for (i = 0; i < n; i++) {
        char bf[1024], *q;
        sprintf(bf, "line %ld\n", i);
        if ((p = strdup(bf)) == NULL) {
            fprintf(stderr, "Error duplicating string\n");
            return -1;
        }
        if (!ll->set(ll, i, p, (void **)&q)) {
            fprintf(stderr, "Error replacing %ld'th element\n", i);
            return -1;
        }
        free(q);
    }
    /*
     * test of toArray
     */
    printf("===== test of toArray\n");
    ll->lock(ll);
    if ((array = (char **)ll->toArray(ll, &n)) == NULL) {
        fprintf(stderr, "Error in invoking ll->toArray()\n");
        return -1;
    }
    for (i = 0; i < n; i++) {
        printf("%s", array[i]);
    }
    ll->unlock(ll);
    free(array);
    /*
     * test of iterator
     */
    printf("===== test of iterator\n");
    if ((it = ll->itCreate(ll)) == NULL) {
        fprintf(stderr, "Error in creating iterator\n");
        return -1;
    }
    while (it->hasNext(it)) {
        char *p;
        (void) it->next(it, (void **)&p);
        printf("%s", p);
    }
    it->destroy(it);
    /*
     * test of destroy with free() as freeFxn
     */
    printf("===== test of destroy(free)\n");
    ll->destroy(ll, free);

    return 0;
}
