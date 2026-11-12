/*
 * Test for OpenSimplex2F 1D noise
 */
#include <stdio.h>
#include <math.h>
#include "OpenSimplex2F.h"

int main(void)
{
    struct OpenSimplex2F_context *ctx;
    int64_t seed = 12345;
    double val;
    
    if (OpenSimplex2F(seed, &ctx) != 0) {
        fprintf(stderr, "Failed to initialize\n");
        return 1;
    }
    printf("Testing OpenSimplex2F_noise1...\n");
    val = OpenSimplex2F_noise1(ctx, 0.0); printf("noise1(0.0) = %.6f\n", val);
    val = OpenSimplex2F_noise1(ctx, 0.5); printf("noise1(0.5) = %.6f\n", val);
    val = OpenSimplex2F_noise1(ctx, 1.0); printf("noise1(1.0) = %.6f\n", val);
    val = OpenSimplex2F_noise1(ctx, -0.5); printf("noise1(-0.5) = %.6f\n", val);
    val = OpenSimplex2F_noise1(ctx, 5.5); printf("noise1(5.5) = %.6f\n", val);
    val = OpenSimplex2F_noise1(ctx, -5.5); printf("noise1(-5.5) = %.6f\n", val);
    OpenSimplex2F_free(ctx);
    printf("All tests passed!\n");
    return 0;
}
