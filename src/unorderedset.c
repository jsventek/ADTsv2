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
#include <stdlib.h>
#include <string.h>

#define DEFAULT_CAPACITY 16L
#define MAX_CAPACITY 134217728L
#define DEFAULT_LOAD_FACTOR 0.75
#define TRIGGER 100	/* number of changes that will trigger a load check */

typedef struct entry {
    struct entry *next;
    void *element;
} Entry;

typedef struct us_data {
    long size;
    long capacity;
    long changes;
    double load;
    double loadFactor;
    double increment;
    int (*cmp)(void *, void *);
    long (*hash)(void *, long);
    Entry **buckets;
} UsData;

/*
 * traverses the hashset, calling freeFxn on each element
 * then frees storage associated with the key and the Entry structure
 */
static void purge(UsData *usd, void (*freeFxn)(void *element)) {

    long i;

    for (i = 0L; i < usd->capacity; i++) {
        Entry *p, *q;
        p = usd->buckets[i];
        while (p != NULL) {
            if (freeFxn != NULL)
                (*freeFxn)(p->element);
            q = p->next;
            free(p);
            p = q;
        }
        usd->buckets[i] = NULL;
    }
}

static void us_destroy(const UnorderedSet *us, void (*freeFxn)(void *element)) {
    UsData *usd = (UsData *)us->self;
    purge(usd, freeFxn);
    free(usd->buckets);
    free(usd);
    free((void *)us);
}

static void us_clear(const UnorderedSet *us, void (*freeFxn)(void *element)) {
    UsData *usd = (UsData *)us->self;
    purge(usd, freeFxn);
    usd->size = 0;
    usd->load = 0.0;
    usd->changes = 0;
}

/*
 * local function to locate entry in a hashset
 *
 * returns pointer to entry, if found, as function value; NULL if not found
 * returns bucket index in `*bucket'
 */
static Entry *findEntry(UsData *usd, void *element, long *bucket) {
    long i = usd->hash(element, usd->capacity);
    Entry *p;

    *bucket = i;
    for (p = usd->buckets[i]; p != NULL; p = p->next) {
        if (usd->cmp(p->element, element) == 0) {
            break;
        }
    }
    return p;
}

/*
 * local function that resizes the hashset
 */
static void resize(UsData *usd) {
    int N;
    Entry *p, *q, **array;
    long i, j;

    N = 2 * usd->capacity;
    if (N > MAX_CAPACITY)
        N = MAX_CAPACITY;
    if (N == usd->capacity)
        return;
    array = (Entry **)malloc(N * sizeof(Entry *));
    if (array == NULL)
        return;
    for (j = 0; j < N; j++)
        array[j] = NULL;
    /*
     * now redistribute the entries into the new set of buckets
     */
    for (i = 0; i < usd->capacity; i++) {
        for (p = usd->buckets[i]; p != NULL; p = q) {
            q = p->next;
            j = usd->hash(p->element, N);
            p->next = array[j];
            array[j] = p;
        }
    }
    free(usd->buckets);
    usd->buckets = array;
    usd->capacity = N;
    usd->load /= 2.0;
    usd->changes = 0;
    usd->increment = 1.0 / (double)N;
}

static int us_add(const UnorderedSet *us, void *element) {
    UsData *usd = (UsData *)us->self;
    long i;
    Entry *p;
    int ans = 0;

    if (usd->changes > TRIGGER) {
        usd->changes = 0;
        if (usd->load > usd->loadFactor)
            resize(usd);
    }
    p = findEntry(usd, element, &i);
    if (p == NULL) {	/* element does not exist in set */
        p = (Entry *)malloc(sizeof(Entry));
        if (p != NULL) {
            p->element = element;
            p->next = usd->buckets[i];
            usd->buckets[i] = p;
            usd->size++;
            usd->load += usd->increment;
            usd->changes++;
            ans = 1;
        } else {
            free(p);
        }
    }
    return ans;
}

static int us_contains(const UnorderedSet *us, void *element) {
    UsData *usd = (UsData *)us->self;
    long bucket;

    return (findEntry(usd, element, &bucket) != NULL);
}

static int us_isEmpty(const UnorderedSet *us) {
    UsData *usd = (UsData *)us->self;
    return (usd->size == 0L);
}

static int us_remove(const UnorderedSet *us, void *element, void (*freeFxn)(void*)) {
    UsData *usd = (UsData *)us->self;
    long i;
    Entry *entry;
    int ans = 0;

    entry = findEntry(usd, element, &i);
    if (entry != NULL) {
        Entry *p, *c;
        /* determine where the entry lives in the singly linked list */
        for (p = NULL, c = usd->buckets[i]; c != entry; p = c, c = c->next)
            ;
        if (p == NULL)
            usd->buckets[i] = entry->next;
        else
            p->next = entry->next;
        usd->size--;
        usd->load -= usd->increment;
        usd->changes++;
        if (freeFxn != NULL)
            (*freeFxn)(entry->element);
        free(entry);
        ans = 1;
    }
    return ans;
}

static long us_size(const UnorderedSet *us) {
    UsData *usd = (UsData *)us->self;
    return usd->size;
}

/*
 * local function for generating an array of void * from a hashset
 *
 * returns pointer to the array or NULL if malloc failure
 */
static void **entries(UsData *usd) {
    void **tmp = NULL;
    if (usd->size > 0L) {
        size_t nbytes = usd->size * sizeof(void *);
        tmp = (void **)malloc(nbytes);
        if (tmp != NULL) {
            long i, n = 0L;
            for (i = 0L; i < usd->capacity; i++) {
                Entry *p;
                p = usd->buckets[i];
                while (p != NULL) {
                    tmp[n++] = p->element;
                    p = p->next;
                }
            }
        }
    }
    return tmp;
}

static void **us_toArray(const UnorderedSet *us, long *len) {
    UsData *usd = (UsData *)us->self;
    void **tmp = entries(usd);

    if (tmp != NULL)
        *len = usd->size;
    return tmp;
}

static const Iterator *us_itCreate(const UnorderedSet *us) {
    UsData *usd = (UsData *)us->self;
    const Iterator *it = NULL;
    void **tmp = entries(usd);

    if (tmp != NULL) {
        it = Iterator_create(usd->size, tmp);
        if (it == NULL)
            free(tmp);
    }
    return it;
}

static UnorderedSet template = {
    NULL, us_destroy, us_clear, us_add, us_contains, us_isEmpty,
    us_remove, us_size, us_toArray, us_itCreate
};

const UnorderedSet *UnorderedSet_create(
                              int (*cmpFn)(void*,void*),
                              long (*hashFn)(void*,long),
                              long capacity, double loadFactor
) {
    UnorderedSet *us = (UnorderedSet *)malloc(sizeof(UnorderedSet));

    if (us != NULL) {
        UsData *usd = (UsData *)malloc(sizeof(UsData));
        if (usd != NULL) {
            long N;
            double lf;
            Entry **array;
            long i;
            N = ((capacity > 0) ? capacity : DEFAULT_CAPACITY);
            if (N > MAX_CAPACITY)
                N = MAX_CAPACITY;
            lf = ((loadFactor > 0.000001) ? loadFactor : DEFAULT_LOAD_FACTOR);
            array = (Entry **)malloc(N * sizeof(Entry *));
            if (array != NULL) {
                usd->capacity = N;
                usd->loadFactor = lf;
                usd->size = 0L;
                usd->load = 0.0;
                usd->changes = 0L;
                usd->increment = 1.0 / (double)N;
                usd->cmp = cmpFn;
                usd->hash = hashFn;
                usd->buckets = array;
                for (i = 0; i < N; i++)
                    array[i] = NULL;
                *us = template;
                us->self = usd;
            } else {
                free(usd);
                free(us);
                us = NULL;
            }
        } else {
            free(us);
            us = NULL;
        }
    }
    return us;
}
