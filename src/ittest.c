#include "iterator.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    char *sp;
    char **elements = (char **)malloc(3*sizeof(char *));

    elements[0] = "line 1";
    elements[1] = "line 2";
    elements[2] = "line 3";
    Iterator *it = Iterator_create(3, (void **)elements);
    while (it->hasNext(it)) {
        (void)it->next(it, (void **)&sp);
        printf("%s\n", sp);
    }
    it->destroy(it);
    return 0;
}
