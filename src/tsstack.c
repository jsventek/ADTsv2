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
 * implementation for thread-safe generic stack
 */

#include "tsstack.h"
#include "stack.h"
#include <stdlib.h>
#include <pthread.h>

#define LOCK(st) &((st)->lock)

typedef struct tsst_data {
    const Stack *st;
    pthread_mutex_t lock;	/* this is a recursive lock */
} TSStData;

static void st_destroy(const TSStack *st, void (*freeFxn)(void *element)) {
    TSStData *std = (TSStData *)st->self;

    pthread_mutex_lock(LOCK(std));
    std->st->destroy(std->st, freeFxn);
    pthread_mutex_unlock(LOCK(std));
    pthread_mutex_destroy(LOCK(std));
    free(std);
    free((void *)st);
}

static void st_clear(const TSStack *st, void (*freeFxn)(void *element)) {
    TSStData *std = (TSStData *)st->self;

    pthread_mutex_lock(LOCK(std));
    std->st->clear(std->st, freeFxn);
    pthread_mutex_unlock(LOCK(std));
}

static void st_lock(const TSStack *st) {
    TSStData *std = (TSStData *)st->self;

    pthread_mutex_lock(LOCK(std));
}

static void st_unlock(const TSStack *st) {
    TSStData *std = (TSStData *)st->self;

    pthread_mutex_unlock(LOCK(std));
}

static int st_push(const TSStack *st, void *element) {
    TSStData *std = (TSStData *)st->self;
    int result;

    pthread_mutex_lock(LOCK(std));
    result = std->st->push(std->st, element);
    pthread_mutex_unlock(LOCK(std));
    return result;
}

static int st_pop(const TSStack *st, void **element) {
    TSStData *std = (TSStData *)st->self;
    int result;

    pthread_mutex_lock(LOCK(std));
    result = std->st->pop(std->st, element);
    pthread_mutex_unlock(LOCK(std));
    return result;
}

static int st_peek(const TSStack *st, void **element) {
    TSStData *std = (TSStData *)st->self;
    int result;

    pthread_mutex_lock(LOCK(std));
    result = std->st->peek(std->st, element);
    pthread_mutex_unlock(LOCK(std));
    return result;
}

static long st_size(const TSStack *st) {
    TSStData *std = (TSStData *)st->self;
    long result;

    pthread_mutex_lock(LOCK(std));
    result = std->st->size(std->st);
    pthread_mutex_unlock(LOCK(std));
    return result;
}

static int st_isEmpty(const TSStack *st) {
    TSStData *std = (TSStData *)st->self;
    int result;

    pthread_mutex_lock(LOCK(std));
    result = std->st->isEmpty(std->st);
    pthread_mutex_unlock(LOCK(std));
    return result;
}

static void **st_toArray(const TSStack *st, long *len) {
    TSStData *std = (TSStData *)st->self;
    void **result;

    pthread_mutex_lock(LOCK(std));
    result = std->st->toArray(std->st, len);
    pthread_mutex_unlock(LOCK(std));
    return result;
}

static const TSIterator *st_itCreate(const TSStack *st) {
    TSStData *std = (TSStData *)st->self;
    const TSIterator *it = NULL;
    void **tmp;
    long len;

    pthread_mutex_lock(LOCK(std));
    tmp = std->st->toArray(std->st, &len);
    if (tmp != NULL) {
        it = TSIterator_create(LOCK(std), len, tmp);
        if (it == NULL)
            free(tmp);
    }
    if (it == NULL)
        pthread_mutex_unlock(LOCK(std));
    return it;
}

static TSStack template = {
    NULL, st_destroy, st_clear, st_lock, st_unlock, st_push, st_pop, st_peek,
    st_size, st_isEmpty, st_toArray, st_itCreate
};

const TSStack *TSStack_create(long capacity) {
    TSStack *st = (TSStack *)malloc(sizeof(TSStack));

    if (st != NULL) {
        TSStData *std = (TSStData *)malloc(sizeof(TSStData));

        if (std != NULL) {
            std->st = Stack_create(capacity);

            if (std->st != NULL) {
                pthread_mutexattr_t ma;
                pthread_mutexattr_init(&ma);
                pthread_mutexattr_settype(&ma, PTHREAD_MUTEX_RECURSIVE);
                pthread_mutex_init(LOCK(std), &ma);
                pthread_mutexattr_destroy(&ma);
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
