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

/*
 * implementation for generic stack
 */

#include "stack.h"
#include <stdlib.h>

#define DEFAULT_CAPACITY 50L
#define MAX_INIT_CAPACITY 1000L

typedef struct st_data {
    long capacity;
    long delta;
    long next;
    void **theArray;
} StData;

/*
 * local function - traverses stack, applying user-supplied function
 * to each element; if freeFxn is NULL, nothing is done
 */

static void purge(StData *std, void (*freeFxn)(void*)) {
    if (freeFxn != NULL) {
        long i;

        for (i = 0L; i < std->next; i++)
            (*freeFxn)(std->theArray[i]);	/* user frees elem storage */
    }
}

static void st_destroy(const Stack *st, void (*freeFxn)(void *element)) {
    StData *std = (StData *)st->self;

    purge(std, freeFxn);
    free(std->theArray);		/* free array of pointers */
    free(std);				/* free structure with instance data */
    free((void *)st);			/* free dispatch table */
}

static void st_clear(const Stack *st, void (*freeFxn)(void *element)) {
    StData *std = (StData *)st->self;

    purge(std, freeFxn);
    std->next = 0L;
}

static int st_push(const Stack *st, void *element) {
    StData *std = (StData *)st->self;
    int status = 1;

    if (std->capacity <= std->next) {	/* need to reallocate */
        size_t nbytes = (std->capacity + std->delta) * sizeof(void *);
        void **tmp = (void **)realloc(std->theArray, nbytes);

        if (tmp == NULL)
            status = 0;			/* allocation failure */
        else {
            std->theArray = tmp;
            std->capacity += std->delta;
        }
    }
    if (status)
        std->theArray[std->next++] = element;
    return status;
}

static int st_pop(const Stack *st, void **element) {
    StData *std = (StData *)st->self;
    int status = 0;

    if (std->next > 0L) {
        *element = std->theArray[--std->next];
        status = 1;
    }
    return status;
}

static int st_peek(const Stack *st, void **element) {
    StData *std = (StData *)st->self;
    int status = 0;

    if (std->next > 0L) {
        *element = std->theArray[std->next - 1];
        status = 1;
    }
    return status;
}

static long st_size(const Stack *st) {
    StData *std = (StData *)st->self;

    return std->next;
}

static int st_isEmpty(const Stack *st) {
    StData *std = (StData *)st->self;

    return (std->next == 0L);
}

/*
 * local function - duplicates array of void * pointers on the heap
 *
 * returns pointers to duplicate array or NULL if malloc failure
 */
static void **arrayDupl(StData *std) {
    void **tmp = NULL;

    if (std->next > 0L) {
        size_t nbytes = std->next * sizeof(void *);
        tmp = (void **)malloc(nbytes);
        if (tmp != NULL) {
            long i;

            for (i = 0L; i < std->next; i++)
                tmp[i] = std->theArray[i];
        }
    }
    return tmp;
}

static void **st_toArray(const Stack *st, long *len) {
    StData *std = (StData *)st->self;
    void **tmp = arrayDupl(std);

    if (tmp != NULL)
        *len = std->next;
    return tmp;
}

static const Iterator *st_itCreate(const Stack *st) {
    StData *std = (StData *)st->self;
    const Iterator *it = NULL;
    void **tmp = arrayDupl(std);

    if (tmp != NULL) {
        it = Iterator_create(std->next, tmp);
        if (it == NULL)
            free(tmp);
    }
    return it;
}

static Stack template = {
    NULL, st_destroy, st_clear, st_push, st_pop, st_peek, st_size,
    st_isEmpty, st_toArray, st_itCreate
};

const Stack *Stack_create(long capacity) {
    Stack *st = (Stack *)malloc(sizeof(Stack));

    if (st != NULL) {
        StData *std = (StData *)malloc(sizeof(StData));

        if (std != NULL) {
            long cap;
            void **array = NULL;

            cap = (capacity <= 0L) ? DEFAULT_CAPACITY : capacity;
            cap = (cap > MAX_INIT_CAPACITY) ? MAX_INIT_CAPACITY : cap;
            array = (void **)malloc(cap * sizeof(void *));
            if (array != NULL) {
                std->capacity = cap;
                std->delta = cap;
                std->next = 0L;
                std->theArray = array;
                *st = template;
                st->self = std;
            } else {
                free(std);
                free(st);
                st = NULL;
            }
        } else {
            free(st);
            st = NULL;
        }
    }
    return st;
}
