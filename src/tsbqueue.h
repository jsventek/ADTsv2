#ifndef _TSBQUEUE_H_
#define _TSBQUEUE_H_

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
 * interface definition for generic, thread-safe bounded FIFO queue
 *
 * patterned roughly after Java 6 Queue interface
 */

#include "tsiterator.h"			/* needed for factory method */

typedef struct tsbqueue TSBQueue;	/* forward reference */

/*
 * create an bounded queue; if capacity is 0L, give it a default capacity
 *
 * returns a pointer to the queue, or NULL if there are malloc() errors
 */
const TSBQueue *TSBQueue_create(long capacity);

/*
 * now define struct tsbqueue
 */
struct tsbqueue {
/*
 * private data of the bounded queue
 */
    void *self;

/*
 * destroys the bounded queue; for each element, if freeFxn != NULL,
 * invokes freeFxn on the element; then returns any queue structure
 * associated with the element; finally, deletes any remaining structures
 * associated with the queue
 */
    void (*destroy)(const TSBQueue *tsbq, void (*freeFxn)(void *element));

/*
 * clears the queue; for each element, if freeFxn != NULL, invokes
 * freeFxn on the element; then returns any queue structure associated with
 * the element
 *
 * upon return, the queue is empty
 */
    void (*clear)(const TSBQueue *tsbq, void (*freeFxn)(void *element));

/*
 * obtains the lock for exclusive access
 */
    void (*lock)(const TSBQueue *uq);

/*
 * returns the lock
 */
    void (*unlock)(const TSBQueue *uq);

/*
 * non-blocking append of `element' to the end of the bounded queue
 *
 * returns 1 if successful, 0 if unsuccesful (queue is full)
 */
    int (*add)(const TSBQueue *tsbq, void *element);

/*
 * blocking append of `element' to the end of the bounded queue
 */
    void (*put)(const TSBQueue *tsbq, void *element);

/*
 * non-blocking retrieval, but does not remove, the head of the queue
 *
 * returns 1 if successful, 0 if unsuccessful (queue is empty)
 */
    int (*peek)(const TSBQueue *tsbq, void **element);

/*
 * non-blocking retrieval, and removal, of the head of the queue
 *
 * return 1 if successful, 0 if not (queue is empty)
 */
    int (*remove)(const TSBQueue *tsbq, void **element);

/*
 * blocking retrieval, and removal, of the head of the queue
 */
    void (*take)(const TSBQueue *tsbq, void **element);

/*
 * returns the number of elements in the queue
 */
    long (*size)(const TSBQueue *tsbq);

/*
 * returns true if the queue is empty, false if not
 */
    int (*isEmpty)(const TSBQueue *tsbq);

/*
 * returns an array containing all of the elements of the queue in
 * proper sequence (from first to last element); returns the length of the
 * queue in `len'
 *
 * returns pointer to void * array of elements, or NULL if malloc failure
 */
    void **(*toArray)(const TSBQueue *tsbq, long *len);

/*
 * creates an iterator for running through the queue
 *
 * returns pointer to the Iterator or NULL
 */
    const TSIterator *(*itCreate)(const TSBQueue *tsbq);
};

#endif /* _TSBQUEUE_H_ */
