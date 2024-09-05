#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"

#define PROFILE_ITERATIONS 100000

#define FIXEDPT_BITS 32

//#define FIXEDPT_WBITS 8

#define _FIXEDPT_IMPLEMENTATION
#include "fixedptc.h"

/* This test program verifies the fixedpt precision, comparing it to
 * float and double precision results. */

static const float pi_f = 3.14159265358979323846264338;
static const double pi_d = 3.14159265358979323846264338;
static const fixedpt pi_x = fixedpt_rconst(3.14159265358979323846264338);

static const float e_f = 2.71828182845904523536028747;
static const double e_d = 2.71828182845904523536028747;
static const fixedpt e_x = fixedpt_rconst(2.71828182845904523536028747);

static const float atan_1_f = 0.0;
static const float atan_2_f = 0.5;
static const float atan_3_f = 0.707106781;
static const float atan_4_f = 1.0;
static const float atan_5_f = 0.866025403;

static const double atan_1_d = 0.0;
static const double atan_2_d = 0.5;
static const double atan_3_d = 0.707106781;
static const double atan_4_d = 1.0;
static const double atan_5_d = 0.866025403;

static const fixedpt atan_1_x = fixedpt_rconst(0.0);
static const fixedpt atan_2_x = fixedpt_rconst(0.5);
static const fixedpt atan_3_x = fixedpt_rconst(0.707106781);
static const fixedpt atan_4_x = fixedpt_rconst(1.0);
static const fixedpt atan_5_x = fixedpt_rconst(0.866025403);

static const fixedpt x_profile = fixedpt_rconst(1000);

// Function to measure the execution time of a function in microseconds
uint64_t measure_time_us(void (*func)()) {
    uint64_t start_time = time_us_64();
    func();
    uint64_t end_time = time_us_64();
    return end_time - start_time;
}

// Example math functions
void calculate_orig_sin() {
    volatile double result;
    for (int i = 0; i < PROFILE_ITERATIONS; i++) {
        result = sin(e_d);
    }
}

void calculate_sin() {
    volatile fixedpt result;
    for (int i = 0; i < PROFILE_ITERATIONS; i++) {
        result = fixedpt_sin(e_x);
    }
}

void calculate_orig_log() {
    volatile double result;
    for (int i = 0; i < PROFILE_ITERATIONS; i++) {
        result = log(1000.0);
    }
}

void calculate_log() {
    volatile fixedpt result;
    for (int i = 0; i < PROFILE_ITERATIONS; i++) {
        result = fixedpt_ln(x_profile);
    }
}

void calculate_orig_sqrt() {
    volatile double result;
    for (int i = 0; i < PROFILE_ITERATIONS; i++) {
        result = sqrt(1000.0);
    }
}

void calculate_sqrt() {
    volatile fixedpt result;
    for (int i = 0; i < PROFILE_ITERATIONS; i++) {
        result = fixedpt_sqrt(x_profile);
    }
}

int main() {
    uint64_t time_dif;
    stdio_init_all();

    while (1)
    {
        printf("Profiling Math Functions on Raspberry Pi Pico\n");

        time_dif = measure_time_us(calculate_orig_sin);
        printf("Time taken by floating-point sin function: %llu microseconds\n", time_dif);

        time_dif = measure_time_us(calculate_sin);
        printf("Time taken by sin function: %llu microseconds\n", time_dif);

        time_dif = measure_time_us(calculate_orig_log);
        printf("Time taken by floating-point log function: %llu microseconds\n", time_dif);

        time_dif = measure_time_us(calculate_log);
        printf("Time taken by log function: %llu microseconds\n", time_dif);

        time_dif = measure_time_us(calculate_orig_sqrt);
        printf("Time taken by floating-point sqrt function: %llu microseconds\n", time_dif);

        time_dif = measure_time_us(calculate_sqrt);
        printf("Time taken by sqrt function: %llu microseconds\n", time_dif);

        printf("\n\n\n");
        sleep_ms(5000);
    }
    return 0;
}
