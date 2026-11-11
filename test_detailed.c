#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "OpenSimplex2F.h"
#include "OpenSimplex2F_1D.h"

int main(void) {
    struct OpenSimplex2F_context *ctx;
    int seed = 42;
    int x;
    
    if (OpenSimplex2F_1D(seed, &ctx) != 0) {
        fprintf(stderr, "Failed to initialize\n");
        return 1;
    }
    
    printf("x\t\tvalue\t\tindex\n");
    for (x = 0; x < 20; x++) {
        double pos = (double)x / 10.0;
        double value = OpenSimplex2F_1D_noise(ctx, pos);
        printf("%d\t%.6f\n", x, value);
    }
    
    free(ctx);
    return 0;
}
