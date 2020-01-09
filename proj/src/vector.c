#include <stdlib.h>

#include "include/vector.h"

/* PRIVATE */
static inline void
vector_realloc(vector* vec, size_t reserve)
{
  vec->size = reserve;
  vec->data = realloc(vec->data, sizeof(void*) * reserve);
  if (!vec->data)
    return;

  if (vec->end > vec->size)
    vec->end = vec->size;
}

/* PUBLIC */
/* constructor */
vector*
new_vector()
{
  vector* vec = (vector*)malloc(sizeof(vector));
  if (!vec)
    return NULL;

  vec->data = malloc(sizeof(void*) * DFLT_VEC_SIZE);
  if (!vec->data)
    return NULL;

  vec->size = DFLT_VEC_SIZE;
  /* memset(vec->data, NULL, sizeof(void*) * vec->size); */
  vec->end = 0;
  return vec;
}

/* destructor */
void
free_vector_data(vector* vec)
{
  while (vec->end)
    vector_pop_and_free(vec);
}

void
free_vector(vector* vec)
{
  free_vector_data(vec);
  free(vec->data);
  free(vec);
}

bool
vector_contains(vector* vec, void* elem)
{
  for (size_t i = 0; i < vec->end; ++i) {
    if (vector_at(vec, i) == elem)
      return true;
  }
  return false;
}

/* returns first elem */
void*
vector_begin(vector* vec)
{
  if (vec->end == 0)
    return NULL;

  return vec->data[0];
}

/* returns last elem */
void*
vector_end(vector* vec)
{
  if (vec->end == 0)
    return NULL;

  return vec->data[vec->end - 1];
}

/* max number of elements that can be used before realloc */
size_t
vector_reserved(vector* vec)
{
  return vec->size;
}

/* reserve more elements */
size_t
vector_reserve(vector* vec, size_t reserve)
{
  if (vec->size >= reserve)
    return vec->size;

  vector_realloc(vec, reserve);
  return reserve;
}

/* insert element at the end */
void
vector_push_back(vector* vec, void* elem)
{
  if (!vec->size) // 0 reserved space
    vector_realloc(vec, DFLT_VEC_SIZE);

  /* double alloced space if it is exhausted */
  if (vec->end == vec->size)
    vector_realloc(vec, vec->size * 2);

  vec->data[vec->end] = elem;
  ++vec->end;
}

/* delete element at the end */
void
vector_pop_back(vector* vec)
{
  if (!vec->end)
    return;

  vec->data[vec->end - 1] = NULL;
  --vec->end;
}

void
vector_pop_and_free(vector* vec)
{
  if (!vec->end)
    return;

  free(vec->data[vec->end - 1]);
  vector_pop_back(vec);
}

/* change element at given index */
void
vector_set(vector* vec, size_t i, void* elem)
{
  if (i >= vec->end)
    return;

  vec->data[i] = elem;
}

void
vector_clear(vector* vec)
{
  for (size_t i = 0; i < vec->end; ++i)
    vector_set(vec, i, NULL);
}

/* get element at given index */
void*
vector_at(vector* vec, size_t i)
{
  if (i >= vec->end)
    return NULL;

  return vec->data[i];
}

void
vector_insert(vector* vec, size_t i, void* elem)
{
  if (i >= vec->end)
    return;

  /* alloc new data array */
  void** tempvec;
  if (vec->end == vec->size) {
    tempvec = (void**)malloc(sizeof(void*) * vec->size * 2);
    vec->size *= 2;
  }
  else
    tempvec = (void**)malloc(sizeof(void*) * vec->size);

  /* move elements until the the index to insert */
  for (size_t j = 0; j < i; ++j) {
    tempvec[j] = vec->data[j];
  }
  tempvec[i] = elem;

  for (size_t j = i; j < vec->end; ++j) {
    tempvec[j + 1] = vec->data[j];
  }

  free(vec->data);
  vec->data = tempvec;
  ++vec->end;
}

/* delete element at given index */
void
vector_delete(vector* vec, size_t i)
{
  if (i >= vec->end)
    return;

  void** tempvec = (void**)malloc(sizeof(void*) * vec->size);
  for (size_t j = 0; j < i; ++j) {
    tempvec[j] = vec->data[j];
  }

  for (size_t j = i + 1; j < vec->end; ++j) {
    tempvec[j - 1] = vec->data[j];
  }

  free(vec->data);
  vec->data = tempvec;
  --vec->end;
}
