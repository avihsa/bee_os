/**
**************************************************
* @file dictionary.c
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
/* Includes */
#include <stdlib.h>
#include "utils/dictionary.h"
/* Preprocessor defines */
/* Preprocessor macros */
/* Module intern type definitions */
/* Static module variables */
/* Static module functions (prototypes) */
size_t dictionary_checking(dictionary_t **dictionary);

/* Public functions */
/**
 * @brief Creates an empty dictionary with the given size
 * @param dictionary is a pointer to the newly created dictionary
 * @param size is the maximum permitted size of the dictionary
 * @return 0 on success or greater 0 on error
 * @info On error check for this error:
 *     DICTIONARY_NO_MEMORY: unable to allocate memory for dictionary
 */
size_t dictionary_create(dictionary_t **dictionary, size_t size) {
    (*dictionary) = malloc(sizeof(dictionary_t));
    if (dictionary == NULL) {
        return DICTIONARY_NO_MEMORY;
    }


    (*dictionary)->size  = size;
    (*dictionary)->data = malloc((*dictionary)->size*sizeof(void *));

    if ((*dictionary)->data==NULL) {
        return DICTIONARY_DATA_NO_MEMORY;
    }


    for (size_t i = 0; i<(*dictionary)->size; i++) {
        (*dictionary)->data[i] = NULL;
    }

    return DICTIONARY_SUCCESS;
}


size_t dictionary_delete(dictionary_t **dictionary) {

    size_t status = dictionary_checking(dictionary);

    if (status!=DICTIONARY_SUCCESS) {
        return status;
    }

    free((*dictionary)->data);
    (*dictionary)->data = NULL;
    free((*dictionary));
    (*dictionary) = NULL;


    return DICTIONARY_SUCCESS;
}

/**
 * @brief Adds or overwrites an entry to the dictionary.
 * @param dictionary is pointer to the dictionary to be added to
 * @param key is the index value of the newly created entry
 * @param value is the value of the newly created entry
 * @return 0 on success or greater 0 on error
 * @info On error check for this error:
 *     DICTIONARY_OVERFLOW: unable to create new entry, key is out of bounds for max size
 */
size_t dictionary_add(dictionary_t **dictionary, size_t key, void **value) {

    size_t status = dictionary_checking(dictionary);

    if (status!=DICTIONARY_SUCCESS) {
        return status;
    }

    if (key >= (*dictionary)->size) {
        return DICTIONARY_OVERFLOW;
    }


    (*dictionary)->data[key] = *value;

    return DICTIONARY_SUCCESS;
}

/**
 * @brief Reads the value of a specified entry in the dictionary
 * @param dictionary is a pointer to the dictionary to be read from
 * @param key is the index value of the entry to read
 * @param value is a pointer to data memory expecting the data of the read entry
 * @return 0 on success or greater 0 on error
 * @info Currently no error cases.
 */
size_t dictionary_get(dictionary_t **dictionary, size_t key, void **value) {

    size_t status = dictionary_checking(dictionary);

    if (status!=DICTIONARY_SUCCESS) {
        return status;
    }



    (*value) = (*dictionary)->data[key];

    if ((*value)==NULL) {
        return DICTIONARY_VALUE_IS_NULL;
    }

    return DICTIONARY_SUCCESS;
}

/* Static module functions (implementation) */


size_t dictionary_checking(dictionary_t **dictionary) {

    if ((*dictionary) == NULL) {
        return DICTIONARY_NO_MEMORY;
    }
    else if ((*dictionary)->data == NULL) {
        return DICTIONARY_DATA_NO_MEMORY;
    }

    return DICTIONARY_SUCCESS;
}
