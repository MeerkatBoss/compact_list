/**
 * @file cringe.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 * @brief Additional list functions
 * 
 * @par Preamble
 * Due to the fact that getting element from a list by its logical
 * number is famously difficult, this task has been solved using
 * design patterns of object-oriented programming. This was complicated
 * somewhat by the specifics of C programming language, which is not
 * object-oriented. Nevertheless, I managed to implement several famous
 * design patters, namely Abstract Factory, Builder and Iterator.
 * 
 * @par Usage
 * @parblock
 * This file contains for functions, which need to be called
 * successively. Each functions requires GUID passed as the last
 * parameter. GUIDs are presented in the following photos:
 * 
 * @image html 1.jpg
 * @image html 2.jpg
 * @image html 3.jpg
 * @image html 4.jpg
 * 
 * @endparblock
 * 
 * @see "Design Patterns: Elements of Reusable Object-Oriented Software"
 * by E. Gamma, R. Helm, R. Johnson and J. Vlissides
 * 
 * @note The third to last GUID contains a typo: third and fourth
 * blocks of digits have been accidentally swapped
 * @warning Attempt to call any function with wrong parameters will
 * lead to undefined behaviour
 * 
 * 
 * @version 0.1
 * @date 2022-10-29
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef CRINGE_H
#define CRINGE_H

/**
 * @brief Create iterator getter
 * 
 * @return Iterator getter
 */
void* abstract_linked_list_iterator_object_getter_factory_make_object_getter(void*, void*);

/**
 * @brief Get abstract factory builder
 * 
 * @return Builder
 */
void* abstract_linked_list_iterator_object_getter_factory_builder_getter_get_builder(void*, void*, void*);

/**
 * @brief Get list iterator
 * 
 * @return Iterator
 */
void* abstract_linked_list_iterator_object_getter_get_abstract_iterator_object(void*, void*);

/**
 * @brief Build factory
 * 
 * @return Factory
 */
void* abstract_linked_list_iterator_object_getter_factory_builder_build_factory(void*, void*);



#endif