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

#include "unorderedset.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int scmp(void *a, void *b) {
    return strcmp((char *)a, (char *)b);
}

#define SHIFT 7L
static long shash(void *s, long N) {
    long ans = 0L;
    char *sp;

    for (sp = (char *)s; *sp != '\0'; sp++)
        ans = ((SHIFT * ans) + *sp) % N;
    return ans;
}

int main(int argc, char *argv[]) {
    char buf[1024];
    char *p;
    const UnorderedSet *us;
    long i, n;
    FILE *fd;
    const Iterator *it;
    void **array;

    if (argc != 2) {
        fprintf(stderr, "usage: ./ustest file\n");
        return -1;
    }
    if ((us = UnorderedSet_create(scmp, shash, 0L, 0.0)) == NULL) {
        fprintf(stderr, "Error creating hashset of strings\n");
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
        if (!us->add(us, p)) {
            fprintf(stderr, "Duplicate line: \"%s\"\n", p);
            free(p);
        }
    }
    fclose(fd);
    n = us->size(us);
    /*
     * test of remove()
     */
    printf("===== test of remove\n");
    printf("Size before remove = %ld\n", n);
    fd = fopen(argv[1], "r");	/* We know we can open it */
    i = 0;
    while (fgets(buf, 1024, fd) != NULL) {
        p = strchr(buf, '\n');
        *p = '\0';
        printf("%s\n", buf);
        if (!us->remove(us, buf, free)) {
            fprintf(stderr, "Error removing %ld'th element\n", i);
        }
        i++;
    }
    fclose(fd);
    printf("Size after remove = %ld\n", us->size(us));
    /*
     * test of destroy with NULL freeFxn
     */
    printf("===== test of destroy(NULL)\n");
    us->destroy(us, NULL);
    /*
     * recreate hashset
     */
    if ((us = UnorderedSet_create(scmp, shash, 0L, 0.0)) == NULL) {
        fprintf(stderr, "Error creating hashset of strings\n");
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
        if (!us->add(us, p)) {
            free(p);
        }
    }
    fclose(fd);
    /*
     * test of toArray
     */
    printf("===== test of toArray\n");
    if ((array = us->toArray(us, &n)) == NULL) {
        fprintf(stderr, "Error in invoking us->toArray()\n");
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
    if ((it = us->itCreate(us)) == NULL) {
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
     * test of destroy with free() as freeFxn
     */
    printf("===== test of destroy(free)\n");
    us->destroy(us, free);

    return 0;
}
