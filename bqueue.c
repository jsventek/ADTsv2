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
 * implementation for generic bounded FIFO queue
 */

#include "bqueue.h"
#include <stdlib.h>

typedef struct bq_data {
    long count;
    long size;
    int in;
    int out;
    void **buffer;
} BqData;

static void purge(BqData *bqd, void (*freeFxn)(void *element)) {
    if (freeFxn != NULL) {
        int i, n;

        for (i = bqd->out, n = bqd->count; n > 0; i = (i + 1) % bqd->size, n--)
            (*freeFxn)(bqd->buffer[i]);
    }
}

static void bq_destroy(const BQueue *bq, void (*freeFxn)(void *element)) {
    BqData *bqd = (BqData *)bq->self;
    purge(bqd, freeFxn);
    free(bqd->buffer);
    free(bqd);
    free((void *)bq);
}

static void bq_clear(const BQueue *bq, void (*freeFxn)(void *element)) {
    BqData *bqd = (BqData *)bq->self;
    int i;

    purge(bqd, freeFxn);
    for (i = 0; i < bqd->size; i++)
        bqd->buffer[i] = NULL;
    bqd->count = 0;
    bqd->in = 0;
    bqd->out = 0;
}

static int bq_add(const BQueue *bq, void *element) {
    BqData *bqd = (BqData *)bq->self;
    int i;
    
    if (bqd->count == bqd->size)
        return 0;
    i = bqd->in;
    bqd->buffer[i] = element;
    bqd->in = (i + 1) % bqd->size;
    bqd->count++;
    return 1;
}

static int retrieve(BqData *bqd, void **element, int ifRemove) {
    int i;

    if (bqd->count <= 0)
        return 0;
    i = bqd->out;
    *element = bqd->buffer[i];
    if (ifRemove) {
        bqd->out = (i + 1) % bqd->size;
        bqd->count--;
    }
    return 1;
}

static int bq_peek(const BQueue *bq, void **element) {
    BqData *bqd = (BqData *)bq->self;
    return retrieve(bqd, element, 0);
}

static int bq_remove(const BQueue *bq, void **element) {
    BqData *bqd = (BqData *)bq->self;
    return retrieve(bqd, element, 1);
}

static long bq_size(const BQueue *bq) {
    BqData *bqd = (BqData *)bq->self;
    return bqd->count;
}

static int bq_isEmpty(const BQueue *bq) {
    BqData *bqd = (BqData *)bq->self;
    return (bqd->count == 0L);
}

static void **toArray(BqData *bqd) {
    void **tmp = NULL;

    if (bqd->count > 0L) {
        tmp = (void **)malloc(bqd->count * sizeof(void *));
        if (tmp != NULL) {
            int i, j, n;

            n = bqd->count;
            for (i = bqd->out, j = 0; n > 0; i = (i+1) % bqd->size, j++, n--) {
                tmp[j] = bqd->buffer[i];
            }
        }
    }
    return tmp;
}

static void **bq_toArray(const BQueue *bq, long *len) {
    BqData *bqd = (BqData *)bq->self;
    void **tmp = toArray(bqd);

    if (tmp != NULL)
        *len = bqd->count;
    return tmp;
}

static const Iterator *bq_itCreate(const BQueue *bq) {
    BqData *bqd = (BqData *)bq->self;
    const Iterator *it = NULL;
    void **tmp = toArray(bqd);

    if (tmp != NULL) {
        it = Iterator_create(bqd->count, tmp);
        if (it == NULL)
            free(tmp);
    }
    return it;
}

static BQueue template = {
    NULL, bq_destroy, bq_clear, bq_add, bq_peek, bq_remove, bq_size,
    bq_isEmpty, bq_toArray, bq_itCreate
};

const BQueue *BQueue_create(long capacity) {
    BQueue *bq = (BQueue *)malloc(sizeof(BQueue));

    if (bq != NULL) {
        BqData *bqd = (BqData *)malloc(sizeof(BqData));

        if (bqd != NULL) {
            long cap = capacity;
            void **tmp;

            if (cap <= 0L)
                cap = DEFAULT_CAPACITY;
            else if (cap > MAX_CAPACITY)
                cap = MAX_CAPACITY;
            tmp = (void **)malloc(cap * sizeof(void *));
            if (tmp != NULL) {
                int i;

                bqd->count = 0;
                bqd->size = cap;
                bqd->in = 0;
                bqd->out = 0;
                for (i = 0; i < cap; i++)
                    tmp[i] = NULL;
                bqd->buffer = tmp;
                *bq = template;
                bq->self = bqd;
            } else {
                free(bqd);
                free(bq);
                bq = NULL;
            }
        } else {
            free(bq);
            bq = NULL;
        }
    }
    return bq;
}
