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
 * implementation for thread-safe generic bounded queue
 */

#include "tsbqueue.h"
#include "bqueue.h"
#include <stdlib.h>
#include <pthread.h>

#define LOCK(bq) &((bq)->lock)
#define COND(bq) &((bq)->cond)

typedef struct tsbq_data {
    long cap;
    const BQueue *bq;
    pthread_mutex_t lock;	/* this is a recursive lock */
    pthread_cond_t cond;        /* needed for take */
} TSBqData;

static void tsbq_destroy(const TSBQueue *tsbq, void (*freeFxn)(void *element)) {
    TSBqData *bqd = (TSBqData *)tsbq->self;

    pthread_mutex_lock(LOCK(bqd));
    bqd->bq->destroy(bqd->bq, freeFxn);
    pthread_mutex_unlock(LOCK(bqd));
    pthread_mutex_destroy(LOCK(bqd));
    pthread_cond_destroy(COND(bqd));
    free(bqd);
    free((void *)tsbq);
}

static void tsbq_clear(const TSBQueue *tsbq, void (*freeFxn)(void *element)) {
    TSBqData *bqd = (TSBqData *)tsbq->self;

    pthread_mutex_lock(LOCK(bqd));
    bqd->bq->clear(bqd->bq, freeFxn);
    pthread_mutex_unlock(LOCK(bqd));
}

static void tsbq_lock(const TSBQueue *tsbq) {
    TSBqData *bqd = (TSBqData *)tsbq->self;

    pthread_mutex_lock(LOCK(bqd));
}

static void tsbq_unlock(const TSBQueue *tsbq) {
    TSBqData *bqd = (TSBqData *)tsbq->self;

    pthread_mutex_unlock(LOCK(bqd));
}

static int tsbq_add(const TSBQueue *tsbq, void *element) {
    TSBqData *bqd = (TSBqData *)tsbq->self;

    int result;
    pthread_mutex_lock(LOCK(bqd));
    result = bqd->bq->add(bqd->bq, element);
    if (result)
        pthread_cond_signal(COND(bqd));
    pthread_mutex_unlock(LOCK(bqd));
    return result;
}

static void tsbq_put(const TSBQueue *tsbq, void *element) {
    TSBqData *bqd = (TSBqData *)tsbq->self;

    pthread_mutex_lock(LOCK(bqd));
    while (bqd->bq->size(bqd->bq) == bqd->cap)
        pthread_cond_wait(COND(bqd), LOCK(bqd));
    (void)bqd->bq->add(bqd->bq, element);
    pthread_cond_signal(COND(bqd));
    pthread_mutex_unlock(LOCK(bqd));
}

static int tsbq_peek(const TSBQueue *tsbq, void **element) {
    TSBqData *bqd = (TSBqData *)tsbq->self;

    int result;
    pthread_mutex_lock(LOCK(bqd));
    result = bqd->bq->peek(bqd->bq, element);
    pthread_mutex_unlock(LOCK(bqd));
    return result;
}

static int tsbq_remove(const TSBQueue *tsbq, void **element) {
    TSBqData *bqd = (TSBqData *)tsbq->self;

    int result;
    pthread_mutex_lock(LOCK(bqd));
    result = bqd->bq->remove(bqd->bq, element);
    if (result)
        pthread_cond_signal(COND(bqd));
    pthread_mutex_unlock(LOCK(bqd));
    return result;
}

static void tsbq_take(const TSBQueue *tsbq, void **element) {
    TSBqData *bqd = (TSBqData *)tsbq->self;

    pthread_mutex_lock(LOCK(bqd));
    while (bqd->bq->size(bqd->bq) == 0L)
        pthread_cond_wait(COND(bqd), LOCK(bqd));
    (void)bqd->bq->remove(bqd->bq, element);
    pthread_cond_signal(COND(bqd));
    pthread_mutex_unlock(LOCK(bqd));
}

static long tsbq_size(const TSBQueue *tsbq) {
    TSBqData *bqd = (TSBqData *)tsbq->self;

    long result;
    pthread_mutex_lock(LOCK(bqd));
    result = bqd->bq->size(bqd->bq);
    pthread_mutex_unlock(LOCK(bqd));
    return result;
}

static int tsbq_isEmpty(const TSBQueue *tsbq) {
    TSBqData *bqd = (TSBqData *)tsbq->self;

    int result;
    pthread_mutex_lock(LOCK(bqd));
    result = bqd->bq->isEmpty(bqd->bq);
    pthread_mutex_unlock(LOCK(bqd));
    return result;
}

static void **tsbq_toArray(const TSBQueue *tsbq, long *len) {
    TSBqData *bqd = (TSBqData *)tsbq->self;

    void **result;
    pthread_mutex_lock(LOCK(bqd));
    result = bqd->bq->toArray(bqd->bq, len);
    pthread_mutex_unlock(LOCK(bqd));
    return result;
}

static const TSIterator *tsbq_itCreate(const TSBQueue *tsbq) {
    TSBqData *bqd = (TSBqData *)tsbq->self;

    const TSIterator *it = NULL;
    void **tmp;
    long len;

    pthread_mutex_lock(LOCK(bqd));
    tmp = bqd->bq->toArray(bqd->bq, &len);
    if (tmp != NULL) {
        it = TSIterator_create(LOCK(bqd), len, tmp);
        if (it == NULL)
            free(tmp);
    }
    if (it == NULL)
        pthread_mutex_unlock(LOCK(bqd));
    return it;
}

static TSBQueue template = {
    NULL, tsbq_destroy, tsbq_clear, tsbq_lock, tsbq_unlock, tsbq_add,
    tsbq_put, tsbq_peek, tsbq_remove, tsbq_take, tsbq_size, tsbq_isEmpty,
    tsbq_toArray, tsbq_itCreate
};

const TSBQueue *TSBQueue_create(long capacity) {
    TSBQueue *tsbq = (TSBQueue *)malloc(sizeof(TSBQueue));

    if (tsbq != NULL) {
        TSBqData *bqd = (TSBqData *)malloc(sizeof(TSBqData));

        if (bqd != NULL) {
            bqd->bq = BQueue_create(capacity);

            if (bqd->bq != NULL) {
                pthread_mutexattr_t ma;
                long cap = capacity;
                if (cap <= 0L)
                    cap = DEFAULT_CAPACITY;
                else if (cap > MAX_CAPACITY)
                    cap = MAX_CAPACITY;
                bqd->cap = cap;
                pthread_mutexattr_init(&ma);
                pthread_mutexattr_settype(&ma, PTHREAD_MUTEX_RECURSIVE);
                pthread_mutex_init(LOCK(bqd), &ma);
                pthread_mutexattr_destroy(&ma);
                pthread_cond_init(COND(bqd), NULL);
                *tsbq = template;
                tsbq->self = bqd;
            } else {
                free(bqd);
                free(tsbq);
                tsbq = NULL;
            }
        } else {
            free(tsbq);
            tsbq = NULL;
        }
    }
    return tsbq;
}
