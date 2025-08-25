/**
**************************************************
* @file muster.c
* @author Christopher Klein, Ameline Seba
* @version v1.0
* @date 31.03.22
* @brief: Module for using the periphery xyz
@verbatim
==================================================
  ### Resources used ###
  GPIO: PA1-PA5, PG13, PG15
  ADC: ADC1
==================================================
  ### Usage ###
  (#) Call 'muster_init()' to initialize the
  module and all the necessary peripheries
  (#) Call 'muster_calc_sum(a, b)' to calculate
  the sum of a and b
  ...
==================================================
@endverbatim
**************************************************
*/
/* Includes */
#include "muster.h"
/* Preprocessor defines */
/* Preprocessor macros */
/* Module intern type definitions */
/* Static module variables */
/* Static module functions (prototypes) */
void init_gpio(void);
void init_adc(void);
/* Public functions */
/**
 * @brief Initializes the module and all the necessary periphery
 * @param None
 * @return None
 */
int muster_create(void) {
  init_gpio();
  init_adc();
}
/**
 * @brief Calculates the sum of the passed arguments
 * @param a first summand
 * @param b second summand
 * @return sum of \a and \b
 */
int16_t muster_calc_sum(int16_t s16_a, int16_t s16_b) {
  int16_t s16_sum = a + b;
  return sum;
}
/* Static module functions (implementation) */
/**
 * @brief Initializes the necessary GPIOs for this module
 * @param None
 * @return None
 */
void init_gpio(void) {
  /* Some code for initializing the necessary GPIOs */
}
/**
 * @brief Initializes the necessary ADC for this module
 * @param None
 * @return None
 */
void init_adc(void) {
  /* Some code for initializing the necessary ADC */
}
