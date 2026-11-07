/**
 * @file test_dema_filter.c
 * @brief Unit tests for the Double Exponential Moving Average (DEMA) Filter using Ceedling and Unity.
 */

#include "unity.h"
#include "dema_filter.h"

static dema_filter_t filter;

void setUp(void) {
    /* Initialize default state with sample_time_s = 0.01s (100 Hz sampling rate) */
    dema_filter_init(&filter, 0.01f);
}

void tearDown(void) {
}

/* ============================================================================
   1. DEFENSIVE PROGRAMMING & BOUNDARY TESTS (NULL Pointers & Invalid Inputs)
   ============================================================================ */

void test_dema_filter_null_context_guards(void) {
    float out_val = 0.0f;

    TEST_ASSERT_EQUAL_INT32(-1, dema_filter_init(NULL, 0.01f));
    TEST_ASSERT_EQUAL_INT32(-1, dema_filter_init(&filter, 0.0f));
    TEST_ASSERT_EQUAL_INT32(-1, dema_filter_init(&filter, -0.1f));

    TEST_ASSERT_EQUAL_INT32(-1, dema_filter_set_cutoff(NULL, 10.0f));
    TEST_ASSERT_EQUAL_INT32(-1, dema_filter_set_cutoff(&filter, 0.0f));
    TEST_ASSERT_EQUAL_INT32(-1, dema_filter_set_cutoff(&filter, 60.0f)); /* Nyquist limit: fs/2 = 50Hz */

    TEST_ASSERT_EQUAL_INT32(-1, dema_filter_reset(NULL));
    TEST_ASSERT_EQUAL_INT32(-1, dema_filter_update(NULL, 10.0f, &out_val));
    TEST_ASSERT_EQUAL_INT32(-1, dema_filter_update(&filter, 10.0f, NULL));
}

/* ============================================================================
   2. INITIALIZATION & COLD-START BEHAVIOR
   ============================================================================ */

void test_dema_filter_cold_start_initialization(void) {
    float output = 0.0f;
    
    /* First sample should initialize internal states (e1, e2) to match input */
    TEST_ASSERT_EQUAL_INT32(0, dema_filter_update(&filter, 25.5f, &output));
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 25.5f, output);
    TEST_ASSERT_TRUE(filter.is_initialized);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 25.5f, filter.e1_last);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 25.5f, filter.e2_last);
}

void test_dema_filter_reset_clears_state(void) {
    float output = 0.0f;
    
    dema_filter_update(&filter, 50.0f, &output);
    TEST_ASSERT_TRUE(filter.is_initialized);

    TEST_ASSERT_EQUAL_INT32(0, dema_filter_reset(&filter));
    TEST_ASSERT_FALSE(filter.is_initialized);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 0.0f, filter.e1_last);
    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 0.0f, filter.e2_last);
}

/* ============================================================================
   3. CUTOFF FREQUENCY & ALPHA CALCULATION
   ============================================================================ */

void test_dema_filter_cutoff_frequency_calculation(void) {
    /* For fs = 100 Hz (Ts = 0.01s), cutoff = 10 Hz */
    /* tau = 1 / (2 * pi * 10) ≈ 0.0159155 s */
    /* D = tau / 0.01 ≈ 1.59155 samples */
    /* alpha = 1 / (1 + D) ≈ 0.3858 */
    TEST_ASSERT_EQUAL_INT32(0, dema_filter_set_cutoff(&filter, 10.0f));
    TEST_ASSERT_FLOAT_WITHIN(0.01f, 0.3858f, filter.alpha);
}

/* ============================================================================
   4. STEP RESPONSE & STEADY-STATE VERIFICATION
   ============================================================================ */

void test_dema_filter_dc_steady_state_tracking(void) {
    float output = 0.0f;
    dema_filter_set_cutoff(&filter, 5.0f);

    /* Cold start step input */
    dema_filter_update(&filter, 100.0f, &output);

    /* Process constant DC signal for several cycles */
    for (int i = 0; i < 50; i++) {
        dema_filter_update(&filter, 100.0f, &output);
    }

    /* Output should converge exactly to 100.0f with zero DC steady-state error */
    TEST_ASSERT_FLOAT_WITHIN(0.001f, 100.0f, output);
}