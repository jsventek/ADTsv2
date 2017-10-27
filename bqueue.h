#ifndef _BQUEUE_H_
#define _BQUEUE_H_

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
 * interface definition for generic bounded FIFO queue
 *
 * patterned roughly after Java 6 Queue interface
 */

#include "iterator.h"			/* needed for factory method */

/* these are needed here for bqueue.c and tsbqueue.c */
#define DEFAULT_CAPACITY 25L
#define MAX_CAPACITY 10240L

typedef struct bqueue BQueue;		/* forward reference */

/*
 * create a bounded queue; if capacity is 0L, give it a default capacity (25L)
 *
 * returns a pointer to the queue, or NULL if there are malloc() errors
 */
const BQueue *BQueue_create(long capacity);

/*
 * now define struct bqueue
 */
struct bqueue {
/*
 * the private data of the bounded queue
 */
    void *self;

/*
 * destroys the bounded queue; for each element, if freeFxn != NULL,
 * invokes freeFxn on the element; then returns any queue structure
 * associated with the element; finally, deletes any remaining structures
 * associated with the queue
 */
    void (*destroy)(const BQueue *bq, void (*freeFxn)(void *element));

/*
 * clears the queue; for each element, if freeFxn != NULL, invokes
 * freeFxn on the element; then returns any queue structure associated with
 * the element
 *
 * upon return, the queue is empty
 */
    void (*clear)(const BQueue *bq, void (*freeFxn)(void *element));

/*
 * appends `element' to the end of the bounded queue
 *
 * returns 1 if successful, 0 if unsuccesful (queue is full)
 */
    int (*add)(const BQueue *bq, void *element);

/*
 * retrieves, but does not remove, the head of the queue, returning that
 * element in `*element'
 *
 * returns 1 if successful, 0 if unsuccessful (queue is empty)
 */
    int (*peek)(const BQueue *bq, void **element);

/*
 * Retrieves, and removes, the head of the queue, returning that
 * element in `*element'
 *
 * return 1 if successful, 0 if not (queue is empty)
 */
    int (*remove)(const BQueue *bq, void **element);

/*
 * returns the number of elements in the queue
 */
    long (*size)(const BQueue *bq);

/*
 * returns true if the queue is empty, false if not
 */
    int (*isEmpty)(const BQueue *bq);

/*
 * returns an array containing all of the elements of the queue in
 * proper sequence (from first to last element); returns the length of the
 * queue in `*len'
 *
 * returns pointer to void * array of elements, or NULL if malloc failure
 *
 * NB - it is the caller's responsibility to free the void * array when
 *      finished with it
 */
    void **(*toArray)(const BQueue *bq, long *len);

/*
 * creates an iterator for running through the queue
 *
 * returns pointer to the Iterator or NULL
 */
    const Iterator *(*itCreate)(const BQueue *bq);
};

#endif /* _BQUEUE_H_ */
