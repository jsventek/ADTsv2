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

#include "tsarraylist.h"
#include "arraylist.h"
#include <stdlib.h>
#include <pthread.h>

#define LOCK(al) &((al)->lock)

/*
 * implementation for thread-safe generic arraylist
 */

typedef struct tsal_data {
    const ArrayList *al;
    pthread_mutex_t lock;	/* this is a recursive lock */
} TSAlData;

void tsal_destroy(const TSArrayList *al, void (*freeFxn)(void *element)) {
    TSAlData *ald = (TSAlData *)al->self;

    pthread_mutex_lock(LOCK(ald));
    ald->al->destroy(ald->al, freeFxn);
    pthread_mutex_unlock(LOCK(ald));
    pthread_mutex_destroy(LOCK(ald));
    free(ald);
    free((void *)al);
}

void tsal_clear(const TSArrayList *al, void (*freeFxn)(void *element)) {
    TSAlData *ald = (TSAlData *)al->self;

    pthread_mutex_lock(LOCK(ald));
    ald->al->clear(ald->al, freeFxn);
    pthread_mutex_unlock(LOCK(ald));
}

void tsal_lock(const TSArrayList *al) {
    TSAlData *ald = (TSAlData *)al->self;

    pthread_mutex_lock(LOCK(ald));
}

void tsal_unlock(const TSArrayList *al) {
    TSAlData *ald = (TSAlData *)al->self;

    pthread_mutex_unlock(LOCK(ald));
}

int tsal_add(const TSArrayList *al, void *element) {
    TSAlData *ald = (TSAlData *)al->self;

    int result;
    pthread_mutex_lock(LOCK(ald));
    result = ald->al->add(ald->al, element);
    pthread_mutex_unlock(LOCK(ald));
    return result;
}

int tsal_ensureCapacity(const TSArrayList *al, long minCapacity) {
    TSAlData *ald = (TSAlData *)al->self;

    int result;
    pthread_mutex_lock(LOCK(ald));
    result = ald->al->ensureCapacity(ald->al, minCapacity);
    pthread_mutex_unlock(LOCK(ald));
    return result;
}

int tsal_get(const TSArrayList *al, long i, void **element) {
    TSAlData *ald = (TSAlData *)al->self;

    int result;
    pthread_mutex_lock(LOCK(ald));
    result = ald->al->get(ald->al, i, element);
    pthread_mutex_unlock(LOCK(ald));
    return result;
}

int tsal_insert(const TSArrayList *al, long i, void *element) {
    TSAlData *ald = (TSAlData *)al->self;

    int result;
    pthread_mutex_lock(LOCK(ald));
    result = ald->al->insert(ald->al, i, element);
    pthread_mutex_unlock(LOCK(ald));
    return result;
}

int tsal_isEmpty(const TSArrayList *al) {
    TSAlData *ald = (TSAlData *)al->self;

    int result;
    pthread_mutex_lock(LOCK(ald));
    result = ald->al->isEmpty(ald->al);
    pthread_mutex_unlock(LOCK(ald));
    return result;
}

int tsal_remove(const TSArrayList *al, long i, void **element) {
    TSAlData *ald = (TSAlData *)al->self;

    int result;
    pthread_mutex_lock(LOCK(ald));
    result = ald->al->remove(ald->al, i, element);
    pthread_mutex_unlock(LOCK(ald));
    return result;
}

int tsal_set(const TSArrayList *al, void *element, long i, void **previous) {
    TSAlData *ald = (TSAlData *)al->self;

    int result;
    pthread_mutex_lock(LOCK(ald));
    result = ald->al->set(ald->al, element, i, previous);
    pthread_mutex_unlock(LOCK(ald));
    return result;
}

long tsal_size(const TSArrayList *al) {
    TSAlData *ald = (TSAlData *)al->self;

    long result;
    pthread_mutex_lock(LOCK(ald));
    result = ald->al->size(ald->al);
    pthread_mutex_unlock(LOCK(ald));
    return result;
}

void **tsal_toArray(const TSArrayList *al, long *len) {
    TSAlData *ald = (TSAlData *)al->self;

    void **result;
    pthread_mutex_lock(LOCK(ald));
    result = ald->al->toArray(ald->al, len);
    pthread_mutex_unlock(LOCK(ald));
    return result;
}

int tsal_trimToSize(const TSArrayList *al) {
    TSAlData *ald = (TSAlData *)al->self;

    int result;
    pthread_mutex_lock(LOCK(ald));
    result = ald->al->trimToSize(ald->al);
    pthread_mutex_unlock(LOCK(ald));
    return result;
}

const TSIterator *tsal_itCreate(const TSArrayList *al) {
    TSAlData *ald = (TSAlData *)al->self;

    const TSIterator *it = NULL;
    void **tmp;
    long len;

    pthread_mutex_lock(LOCK(ald));
    tmp = ald->al->toArray(ald->al, &len);
    if (tmp != NULL) {
        it = TSIterator_create(LOCK(ald), len, tmp);
        if (it == NULL)
            free(tmp);
    }
    if (it == NULL)
        pthread_mutex_unlock(LOCK(ald));
    return it;
}

static TSArrayList template = {
    NULL, tsal_destroy, tsal_clear, tsal_lock, tsal_unlock, tsal_add,
    tsal_ensureCapacity, tsal_get, tsal_insert, tsal_isEmpty, tsal_remove,
    tsal_set, tsal_size, tsal_toArray, tsal_trimToSize, tsal_itCreate
};

const TSArrayList *TSArrayList_create(long capacity) {
    TSArrayList *tsal = (TSArrayList *)malloc(sizeof(TSArrayList));

    if (tsal != NULL) {
        TSAlData *ald = (TSAlData *)malloc(sizeof(TSAlData));

        if (ald != NULL) {
            ald->al = ArrayList_create(capacity);

            if (ald->al != NULL) {
                pthread_mutexattr_t ma;
                pthread_mutexattr_init(&ma);
                pthread_mutexattr_settype(&ma, PTHREAD_MUTEX_RECURSIVE);
                pthread_mutex_init(LOCK(ald), &ma);
                pthread_mutexattr_destroy(&ma);
                *tsal = template;
                tsal->self = ald;
            } else {
                free(ald);
                free(tsal);
                tsal = NULL;
            }
        } else {
            free(tsal);
            tsal = NULL;
        }
    }
    return tsal;
}
