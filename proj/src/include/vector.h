/** @file vector.h */
#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <stdbool.h>
#include <stdlib.h>

/** @addtogroup	util_grp
 * @{
 */

/** Default starting size of a vector object */
#define DFLT_VEC_SIZE 10

/** Wrapper to push-back an integer to a vector */
#define VECTOR_PUSH_BACK(vec, elem) (vector_push_back(vec, (void*)elem))
/** Wrapper to get an integer from a vector */
#define VECTOR_AT(vec, i) ((int)vector_at(vec, i))

/** @struct vector_t
 *  Vector object.
 */
typedef struct vector_t
{
  void** data; /**< Array that holds the current vector information. */
  size_t size; /**< Array size. */
  size_t end;  /**< How many elements are in the array. */
} vector;

/**
 * @brief   Creates a new vector object.
 * @return  Pointer to the new vector object, on success\n
 *          NULL, otherwise.
 */
vector* new_vector();

/**
 * @brief Free the vector object and all its data.
 * @param vec Vector to free the data from.
 */
void free_vector_data(vector* vec);

/**
 * @brief Free the vector object (leaves the data alloced).
 * @param vec Vector to free.
 */
void free_vector(vector* vec);

/**
 * @brief	Verifies if a given element is present in a given vector.
 *
 * @param vec	  Vector where the search will occur.
 * @param elem	Element to find.
 *
 * @return	True, if the element is present in the vector\n
 *          False, otherwise
 */
bool vector_contains(vector* vec, void* elem);

/**
 * @brief Get a pointer to the first element in the vector.
 *
 * @param vec Vector to get the element from.
 *
 * @return  Pointer to the first element, on success\n
 *          NULL, otherwise.
 */
void* vector_begin(vector* vec);

/**
 * @brief Get a pointer to the last element in the vector.
 *
 * @param vec Vector to get the element from.
 *
 * @return  Pointer to the last element, on success\n
 *          NULL, otherwise.
 */
void* vector_end(vector* vec);

/**
 * @brief Get the current reserved space of a vector.
 *
 * @param vec Vector to get the size from.
 *
 * @return  Number of reserved elements.
 */
size_t vector_reserved(vector* vec);

/**
 * @brief   Attempt to reserve space in a vector.
 * @warning May reserve less elements than asked if there's not enough memory
 * for them.
 *
 * @param vec     Vector to work on.
 * @param reserve Number of elements to reserve.
 *
 * @return  Number of reserved elements.
 */
size_t vector_reserve(vector* vec, size_t reserve);

/**
 * @brief Push-back an element into a given vector.
 *
 * @param vec   Vector to insert element into.
 * @param elem  Element to insert.
 */
void vector_push_back(vector* vec, void* elem);

/**
 * @brief Pop-back a given vector.
 * @param vec   Vector to remove element from.
 */
void vector_pop_back(vector* vec);

/**
 * @brief Pop-back a given vector (and free the element).
 * @param vec   Vector to remove element from.
 */
void vector_pop_and_free(vector* vec);

/**
 * @brief   Set a new value for a vector element.
 * @warning Doesn't free the old element.
 *
 * @param vec   Vector to work with.
 * @param i     Index of the element to change.
 * @param elem  New element to insert in the old one's place.
 */
void vector_set(vector* vec, size_t i, void* elem);

/**
 * @brief	  Sets all pointers of a given vector to NULL
 * @param vec Vector to be cleared
 */
void vector_clear(vector* vec);

/**
 * @brief Get pointer to an element at a given position.
 *
 * @param vec Vector to get the element from.
 * @param i   Index of the element in the vector.
 *
 * @return  Pointer to the element, on success\n
 *          NULL, otherwise.
 */
void* vector_at(vector* vec, size_t i);

/**
 * @brief Insert element into a given index.
 *
 * @param vec   Vector to insert the element into.
 * @param i     Index to insert the element into.
 * @param elem  Element to insert.
 */
void vector_insert(vector* vec, size_t i, void* elem);

/**
 * @brief Remove element in a given index.
 *
 * @param vec Vector to remove the element from.
 * @param i   Index of the element in the vector.
 */
void vector_delete(vector* vec, size_t i);

/** @} */

#endif // __VECTOR_H__
