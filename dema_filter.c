/**
  * @file    dema_filter.c
  * @author  Leonardo Costamagna
  * @brief   Double Exponential Moving Average (DEMA) Digital Filter implementation.
  */

#include "dema_filter.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

/* Core DEMA Filter Operations */

int32_t dema_filter_init(dema_filter_t *filter, float sample_time_s)
{
  if ((filter == NULL) || (sample_time_s <= 0.0f))
  {
    return -1;
  }

  filter->sample_time_s = sample_time_s;
  filter->alpha         = 1.0f; /* Arbitrary initial full-pass alpha */

  return dema_filter_reset(filter);
}

int32_t dema_filter_set_cutoff(dema_filter_t *filter, float cutoff_freq_hz)
{
  float fs;
  float tau;
  float D;
  float alpha;

  if ((filter == NULL) || (cutoff_freq_hz <= 0.0f))
  {
    return -1;
  }

  fs = 1.0f / filter->sample_time_s;

  /* Elementary Nyquist criterion check */
  if (cutoff_freq_hz >= (fs / 2.0f))
  {
    return -1;
  }

  /* Relationship based on RC equivalent time constant (tau = 1 / (2 * pi * fc)) */
  tau = 1.0f / (2.0f * M_PI * cutoff_freq_hz);

  /* Group delay in discrete samples */
  D = tau / filter->sample_time_s;

  /* Calculate optimal alpha */
  alpha = 1.0f / (1.0f + D);

  if ((alpha <= 0.0f) || (alpha > 1.0f))
  {
    return -1;
  }

  filter->alpha = alpha;

  return 0;
}

int32_t dema_filter_reset(dema_filter_t *filter)
{
  if (filter == NULL)
  {
    return -1;
  }

  filter->e1_last        = 0.0f;
  filter->e2_last        = 0.0f;
  filter->is_initialized = false;

  return 0;
}

int32_t dema_filter_update(dema_filter_t *filter, float input, float *output)
{
  float e1;
  float e2;

  if ((filter == NULL) || (output == NULL))
  {
    return -1;
  }

  /* First cycle state initialization to prevent step transients */
  if (!filter->is_initialized)
  {
    filter->e1_last        = input;
    filter->e2_last        = input;
    filter->is_initialized = true;
    *output                = input;
    return 0;
  }

  /* First-order EMA calculation (EMA1) */
  e1 = (filter->alpha * input) + ((1.0f - filter->alpha) * filter->e1_last);

  /* Second-order EMA calculation applied over EMA1 (EMA2) */
  e2 = (filter->alpha * e1) + ((1.0f - filter->alpha) * filter->e2_last);

  /* DEMA calculation: compensates phase lag by scaling 2*EMA1 - EMA2 */
  *output = (2.0f * e1) - e2;

  /* Update internal filter states */
  filter->e1_last = e1;
  filter->e2_last = e2;

  return 0;
}