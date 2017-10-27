#ifndef _TSSTACK_H_
#define _TSSTACK_H_

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
 * interface definition for generic stack
 *
 * patterned roughly after Java 6 Stack interface
 */

#include "tsiterator.h"			/* needed for factory method */

typedef struct tsstack TSStack;		/* forward reference */

/*
 * create a stack with the specified capacity; if capacity == 0L, a
 * default initial capacity (50 elements) is used
 *
 * returns a pointer to the stack, or NULL if there are malloc() errors
 */
const TSStack *TSStack_create(long capacity);

/*
 * now define struct tsstack
 */
struct tsstack {
/*
 * the private data of the stack
 */
    void *self;

/*
 * destroys the stack; for each occupied position, if freeFxn != NULL,
 * it is invoked on the element at that position; the storage associated with
 * the stack is then returned to the heap
 */
    void (*destroy)(const TSStack *st, void (*freeFxn)(void *element));

/*
 * clears all elements from the stack; for each occupied position,
 * if freeFxn != NULL, it is invoked on the element at that position;
 * the stack is then re-initialized
 *
 * upon return, the stack is empty
 */
    void (*clear)(const TSStack *st, void (*freeFxn)(void *element));

/*
 * obtains the lock for exclusive access
 */
    void (*lock)(const TSStack *st);

/*
 * releases the lock
 */
    void (*unlock)(const TSStack *st);

/*
 * pushes `element' onto the stack; if no more room in the stack, it is
 * dynamically resized
 *
 * returns 1 if successful, 0 if unsuccessful (malloc errors)
 */
    int (*push)(const TSStack *st, void *element);

/*
 * pops the element at the top of the stack into `*element'
 *
 * returns 1 if successful, 0 if stack was empty
 */
    int (*pop)(const TSStack *st, void **element);

/*
 * peeks at the top element of the stack without removing it;
 * returned in `*element'
 *
 * return 1 if successful, 0 if stack was empty
 */
    int (*peek)(const TSStack *st, void **element);

/*
 * returns the number of elements in the stack
 */
    long (*size)(const TSStack *st);

/*
 * returns true if the stack is empty, false if not
 */
    int (*isEmpty)(const TSStack *st);

/*
 * returns an array containing all of the elements of the stack in
 * proper sequence (from top to bottom element); returns the length of the
 * array in `*len'
 *
 * returns pointer to void * array of elements, or NULL if malloc failure
 *
 * The array of void * pointers is allocated on the heap, so must be returned
 * by a call to free() when the caller has finished using it.
 */
    void **(*toArray)(const TSStack *st, long *len);

/*
 * creates generic iterator to this stack;
 * successive next calls return elements in the proper sequence (top to bottom)
 *
 * returns pointer to the Iterator or NULL if malloc failure
 */
    const TSIterator *(*itCreate)(const TSStack *st);
};

#endif /* _TSSTACK_H_ */
