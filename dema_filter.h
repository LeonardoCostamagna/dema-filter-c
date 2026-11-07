/**
  * @file    dema_filter.h
  * @author  Leonardo Costamagna
  * @brief   Header file containing API declarations, data types,
  *          and context structures for the Double Exponential Moving Average
  *          (DEMA) Digital Filter module.
  */

#ifndef DEMA_FILTER_H
#define DEMA_FILTER_H

/* Ensures C linkage compatibility when compiled with C++, preventing name mangling. */
#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
  * @defgroup DEMA_Filter_Driver Double Exponential Moving Average Filter Module
  * @{
  */

/**
  * @defgroup DEMA_Filter_Types Data Types & Structures
  * @brief Core structures and return codes for DEMA filter execution.
  * @{
  */

/**
  * @brief DEMA Filter Control and State Context Structure.
  */
typedef struct
{
  float alpha;          /**< Smoothing factor coefficient (0.0 < alpha <= 1.0) */
  float sample_time_s;  /**< Sampling period Ts in seconds */
  float e1_last;        /**< Previous state for first EMA accumulator */
  float e2_last;        /**< Previous state for second EMA accumulator */
  bool  is_initialized; /**< Cold-start tracking flag */
} dema_filter_t;

/**
  * @} DEMA_Filter_Types
  */

/**
  * @defgroup DEMA_Filter_Core_API High-Level Application Interface
  * @brief Primary API function declarations for filter initialization, setup, and updates.
  * @{
  */

/**
  * @brief  Initializes the DEMA filter structure with default operational parameters.
  * @param  filter        Pointer to the DEMA filter instance.
  * @param  sample_time_s Sampling period between measurements in seconds (Ts > 0).
  * @return Execution status (0: Success, <0: Communication failure).
  */
int32_t dema_filter_init(dema_filter_t *filter, float sample_time_s);

/**
  * @brief  Dynamically adjusts the filter behavior by calculating alpha for a target cutoff frequency.
  * @param  filter         Pointer to the DEMA filter instance.
  * @param  cutoff_freq_hz Target low-pass cutoff frequency in Hz (0 < fc < Nyquist).
  * @return Execution status (0: Success, <0: Communication failure).
  */
int32_t dema_filter_set_cutoff(dema_filter_t *filter, float cutoff_freq_hz);

/**
  * @brief  Clears internal history states to remove transience.
  * @param  filter Pointer to the DEMA filter instance.
  * @return Execution status (0: Success, <0: Communication failure).
  */
int32_t dema_filter_reset(dema_filter_t *filter);

/**
  * @brief  Processes and filters a new input sample using DEMA.
  * @param  filter Pointer to the DEMA filter instance.
  * @param  input  Raw input sample value.
  * @param  output Pointer to store the filtered output sample value.
  * @return Execution status (0: Success, <0: Communication failure).
  */
int32_t dema_filter_update(dema_filter_t *filter, float input, float *output);

/**
  * @} DEMA_Filter_Core_API
  */

/**
  * @} DEMA_Filter_Driver
  */

#ifdef __cplusplus
}
#endif

#endif /* DEMA_FILTER_H */