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

#include "tsunorderedset.h"
#include "unorderedset.h"
#include <stdlib.h>
#include <pthread.h>

/*
 * implementation for generic thread-safe treeset implementation
 */

#define LOCK(us) &((us)->lock)

typedef struct tsus_data {
    const UnorderedSet *us;
    pthread_mutex_t lock;
} TSUsData;

static void tsus_destroy(const TSUnorderedSet *us,
                         void (*freeFxn)(void *element)) {
    TSUsData *usd = (TSUsData *)us->self;

    pthread_mutex_lock(LOCK(usd));
    usd->us->destroy(usd->us, freeFxn);
    pthread_mutex_unlock(LOCK(usd));
    pthread_mutex_destroy(LOCK(usd));
    free(usd);
    free((void *)us);
}

static void tsus_clear(const TSUnorderedSet *us,
                       void (*freeFxn)(void *element)) {
    TSUsData *usd = (TSUsData *)us->self;

    pthread_mutex_lock(LOCK(usd));
    usd->us->clear(usd->us, freeFxn);
    pthread_mutex_unlock(LOCK(usd));
}

static void tsus_lock(const TSUnorderedSet *us) {
    TSUsData *usd = (TSUsData *)us->self;

    pthread_mutex_lock(LOCK(usd));
}

static void tsus_unlock(const TSUnorderedSet *us) {
    TSUsData *usd = (TSUsData *)us->self;

    pthread_mutex_unlock(LOCK(usd));
}

static int tsus_add(const TSUnorderedSet *us, void *element) {
    TSUsData *usd = (TSUsData *)us->self;
    int result;

    pthread_mutex_lock(LOCK(usd));
    result = usd->us->add(usd->us, element);
    pthread_mutex_unlock(LOCK(usd));
    return result;
}

static int tsus_contains(const TSUnorderedSet *us, void *element) {
    TSUsData *usd = (TSUsData *)us->self;
    int result;

    pthread_mutex_lock(LOCK(usd));
    result = usd->us->contains(usd->us, element);
    pthread_mutex_unlock(LOCK(usd));
    return result;
}

static int tsus_isEmpty(const TSUnorderedSet *us) {
    TSUsData *usd = (TSUsData *)us->self;
    int result;

    pthread_mutex_lock(LOCK(usd));
    result = usd->us->isEmpty(usd->us);
    pthread_mutex_unlock(LOCK(usd));
    return result;
}

static int tsus_remove(const TSUnorderedSet *us, void *element,
                       void (*freeFxn)(void *element)) {
    TSUsData *usd = (TSUsData *)us->self;
    int result;

    pthread_mutex_lock(LOCK(usd));
    result = usd->us->remove(usd->us, element, freeFxn);
    pthread_mutex_unlock(LOCK(usd));
    return result;
}

static long tsus_size(const TSUnorderedSet *us) {
    TSUsData *usd = (TSUsData *)us->self;
    long result;

    pthread_mutex_lock(LOCK(usd));
    result = usd->us->size(usd->us);
    pthread_mutex_unlock(LOCK(usd));
    return result;
}

static void **tsus_toArray(const TSUnorderedSet *us, long *len) {
    TSUsData *usd = (TSUsData *)us->self;
    void **result;

    pthread_mutex_lock(LOCK(usd));
    result = usd->us->toArray(usd->us, len);
    pthread_mutex_unlock(LOCK(usd));
    return result;
}

static const TSIterator *tsus_itCreate(const TSUnorderedSet *us) {
    TSUsData *usd = (TSUsData *)us->self;
    const TSIterator *it = NULL;
    void **tmp;
    long len;

    pthread_mutex_lock(LOCK(usd));
    tmp = usd->us->toArray(usd->us, &len);
    if (tmp != NULL) {
        it = TSIterator_create(LOCK(usd), len, tmp);
        if (it == NULL)
            free(tmp);
    }
    if (it == NULL)
        pthread_mutex_unlock(LOCK(usd));
    return it;
}

static TSUnorderedSet template = {
    NULL, tsus_destroy, tsus_clear, tsus_lock, tsus_unlock, tsus_add,
    tsus_contains, tsus_isEmpty, tsus_remove, tsus_size,
    tsus_toArray, tsus_itCreate
};

const TSUnorderedSet *TSUnorderedSet_create(
                int (*cmpFunction)(void *, void *),
                long (*hashFunction)(void *, long),
                long capacity, double loadFactor) {
    TSUnorderedSet *tsus = (TSUnorderedSet *)malloc(sizeof(TSUnorderedSet));

    if (tsus != NULL) {
        TSUsData *usd = (TSUsData *)malloc(sizeof(TSUsData));

        if (usd != NULL) {
            usd->us = UnorderedSet_create(cmpFunction, hashFunction,
                                          capacity, loadFactor);
            if (usd->us != NULL) {
                pthread_mutexattr_t ma;
                pthread_mutexattr_init(&ma);
                pthread_mutexattr_settype(&ma, PTHREAD_MUTEX_RECURSIVE);
                pthread_mutex_init(LOCK(usd), &ma);
                pthread_mutexattr_destroy(&ma);
                *tsus = template;
                tsus->self = usd;
            } else {
                free(usd);
                free(tsus);
                tsus = NULL;
            }
        } else {
            free(tsus);
            tsus = NULL;
        }
    }
    return tsus;
}
