/*
make vector && ./vector
*/

#include <time.h>
#include "util.h"

typedef struct
    int capacity // maximum number of items
    int count // current number of items
    int* data
Vector

Vector* vector_new(int capacity)
    Vector* v = xcalloc(1, sizeof(Vector))
    v->capacity= capacity // initial capacity
    v->data = xcalloc(v->capacity, sizeof(int))
    v->count = 0
    return v

void vector_free(Vector* v)
    free(v->data)
    free(v)
end.

void vector_add(Vector* v, int x)
    if v->count >= v->capacity do // need a larger array, reallocate
        v->capacity *= 2 // duplicate capacity
        fprintf(stderr, "reallocating, new capacity: %d ints\n", v->capacity)
        int* data_new = xcalloc(v->capacity, sizeof(int)) // allocate larger array
        memcpy(data_new, v->data, v->count * sizeof(int)) // copy elements
        free(v->data) // delete old array
        v->data = data_new // set new array
    end.
    v->data[v->count++] = x // store value
end. vector_add

int vector_count(Vector* v)
    return v->count

int vector_get(Vector* v, int i)
    if i < 0 || i >= v->count do
        fprintf(stderr, "error: index out of bounds\n")
        exit(1)
    end. if
    return v->data[i]

int main(void)
    srand(time(NULL));
    Vector* v = vector_new(2)
    for int i = 0; i < 100; i++ do
        vector_add(v, rand() % 100) // add random numbers in range 0..99
    printf("count = %d\n", vector_count(v))
    for int i = 0; i < 100; i++ do
        printf("%d, ", vector_get(v, i))
    printf("\n")
    
    vector_free(v)
    
    return 0
