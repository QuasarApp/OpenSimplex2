#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "OpenSimplex2F.h"
#include "OpenSimplex2F_1D.h"

int main(void) {
    struct OpenSimplex2F_context *ctx;
    int seed = 42;
    double min = 1e9, max = -1e9;
    double xsb, xsi, ssi, xi;
    
    if (OpenSimplex2F_1D(seed, &ctx) != 0) {
        fprintf(stderr, "Failed to initialize\n");
        return 1;
    }
    
    printf("x\t\tvalue\t\txsb\t\txsi\t\tindex\n");
    for (int x = 0; x < 100; x++) {
        double pos = (double)x / 10.0;
        double xs = pos * 0.366025403784438646763729;
        xsb = floorf(xs);
        xsi = xs - xsb;
        double ssi = (xsi + xsi) * -0.211324865405187023657952;
        double xi = xsi + ssi;
        
        int index = (int)((xsi + xsi) / 2 + 1);
        if (index < 0) index = 0;
        if (index >= 4) index = 3;
        
        double value = OpenSimplex2F_1D_noise(ctx, pos);
        printf("%d\t%.6f\t%d\t%.6f\t%d\n", x, value, xsb, xsi, index);
        if (value < min) min = value;
        if (value > max) max = value;
    }
    
    printf("\nRange: [%.6f, %.6f]\n", min, max);
    free(ctx);
    return 0;
}
