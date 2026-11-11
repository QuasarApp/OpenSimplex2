/*
 * Copyright (C) 2019-2020 KdotJPG (original work, previous license:
 * CC0 1.0 Universal, public domain dedication).
 *
 * Copyright (C) 2026 QuasarApp Group DOO
 * Licensed under the GNU Lesser General Public License v3.0 (LGPL-3.0),
 * see the accompanying LICENSE file.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include "OpenSimplex2F.h"
#include "OpenSimplex2F_1D.h"

#define TEST_COUNT 100
#define TEST_RANGE 10.0

static int test_1D_noise(void)
{
    int seed = 42;
    struct OpenSimplex2F_context *ctx;
    int x;
    double min_val, max_val;
    
    printf("Testing 1D OpenSimplex2 noise (seed %d)...\n", seed);
    
    if (OpenSimplex2F_1D(seed, &ctx) != 0) {
        fprintf(stderr, "FAIL: Failed to initialize 1D noise context\n");
        return 0;
    }
    
    printf("  Context initialized successfully\n");
    
    min_val = 1e300;
    max_val = -1e300;
    
    for (x = 0; x < TEST_COUNT; x++) {
        double pos = (double)x / TEST_RANGE;
        double value = OpenSimplex2F_1D_noise(ctx, pos);
        
        if (value < min_val) min_val = value;
        if (value > max_val) max_val = value;
        
        if (value < -1.01 || value > 1.01) {
            fprintf(stderr, "  FAIL: Noise value out of range at x=%f: %.6f\n", pos, value);
            free(ctx);
            return 0;
        }
    }
    
    printf("  Generated %d noise values\n", TEST_COUNT);
    printf("  Range: [%.6f, %.6f]\n", min_val, max_val);
    printf("  PASS: All values in valid range [-1, 1]\n");
    
    free(ctx);
    return 1;
}

int main(void)
{
    int result = 0;
    result |= test_1D_noise();
    
    printf("\n1D OpenSimplex2 tests: %s\n", result ? "PASSED" : "FAILED");
    return result ? 0 : 1;
}