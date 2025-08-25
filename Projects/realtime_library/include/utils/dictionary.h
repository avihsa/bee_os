/**
**************************************************
* @file dictionary.h
* @author Christopher-Marcel Klein, Ameline Seba
* @version v1.0
* @date Apr 21, 2025
* @brief Module for creating and using dictionaries
@verbatim
==================================================
  ### Resources used ###
  None
==================================================
  ### Usage ###
  (#) Call 'dictionary_create' to create a dictionary
  (#) Call 'dictionary_add' to add an entry to the dictionary
  (#) Call 'dictionary_get' to get an entry from the dictionary
  (#) Call 'dictionary_delete' to delete a dictionary
  (#) All functions call 'dictionary_checking' to validate
      proper dictionary structure. Refer to this function
      for potential error codes not documented in each
      function.
==================================================
@endverbatim
**************************************************
*/

#ifndef UTILS_DICTIONARY_DICTIONARY_H_
#define UTILS_DICTIONARY_DICTIONARY_H_

/* Includes */
#include <stddef.h>
#include <stdint.h>

/* Public Preprocessor defines */
#define DICTIONARY_SUCCESS	    	0
#define DICTIONARY_NO_MEMORY  		1
#define DICTIONARY_DATA_NO_MEMORY 	2
#define DICTIONARY_OVERFLOW   		3
#define DICTIONARY_VALUE_IS_NULL	4

/* Public Preprocessor macros */
/* Public type definitions */

/// Control information for a dictionary
typedef struct {
	void **data;    ///< data fileds for pointers
	size_t size;    ///< amount of data a dictionary it can store
} dictionary_t;

/* Public functions (prototypes) */
size_t dictionary_create(dictionary_t **dictionary, size_t size);
size_t dictionary_add(dictionary_t **dictionary, size_t key, void **value);
size_t dictionary_get(dictionary_t **dictionary, size_t key, void **value);
size_t dictionary_delete(dictionary_t **dictionary);
#endif /* UTILS_DICTIONARY_DICTIONARY_H_ */
