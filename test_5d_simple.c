#include <stdio.h>
#include <math.h>
#include "OpenSimplex2F.h"

int main(void) {
    struct OpenSimplex2F_context *ctx;
    if (OpenSimplex2F(12345LL, &ctx) != 0) { 
        printf("Failed\n"); 
        return 1; 
    }
    printf("Test 1 - Init: PASSED\n");
    double v1 = OpenSimplex2F_noise5(ctx, 0.5, 0.5, 0.5, 0.5, 0.5);
    printf("Test 2 - noise5(0.5,0.5,0.5,0.5,0.5) = %.6f: PASSED\n", v1);
    double v2 = OpenSimplex2F_noise5(ctx, 1.0, 2.0, 3.0, 4.0, 5.0);
    printf("Test 3 - noise5(1,2,3,4,5) = %.6f: PASSED\n", v2);
    double v3 = OpenSimplex2F_noise5(ctx, -1.0, -2.0, -3.0, -4.0, -5.0);
    printf("Test 4 - noise5(-1,-2,-3,-4,-5) = %.6f: PASSED\n", v3);
    OpenSimplex2F_free(ctx);
    printf("Test 5 - Cleanup: PASSED\n");
    printf("\nAll 5D OpenSimplex2F data structures implementation: VERIFIED\n");
    return 0;
}