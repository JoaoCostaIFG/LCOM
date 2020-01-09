/** @file collisions.h */
#ifndef __COLLISIONS_H__
#define __COLLISIONS_H__

/** @addtogroup object_grp
 * @{
 */

/**
 * @brief Dispatcher for the collisions between objects.
 *
 * @param obj1  First object in collision.
 * @param obj2  Second object in collision.
 */
void collision_dispatcher(void* obj1, void* obj2);

/**@}*/

#endif // __COLLISIONS_H__
