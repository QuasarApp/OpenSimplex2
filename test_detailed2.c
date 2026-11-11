#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "OpenSimplex2F.h"
#include "OpenSimplex2F_1D.h"

int main(void) {
    struct OpenSimplex2F_context *ctx;
    int seed = 42;
    double pos;
    
    if (OpenSimplex2F_1D(seed, &ctx) != 0) {
        fprintf(stderr, "Failed to initialize\n");
        return 1;
    }
    
    printf("pos\t\tvalue\n");
    for (int x = 0; x < 50; x++) {
        pos = (double)x / 100.0;
        double value = OpenSimplex2F_1D_noise(ctx, pos);
        printf("%.2f\t%.6f\n", pos, value);
    }
    
    free(ctx);
    return 0;
}
