#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "OpenSimplex2F.h"
#include "OpenSimplex2F_1D.h"

int main(void) {
    struct OpenSimplex2F_context *ctx;
    int seed = 42;
    double xs, xsi, ssi, xi;
    
    if (OpenSimplex2F_1D(seed, &ctx) != 0) {
        fprintf(stderr, "Failed to initialize\n");
        return 1;
    }
    
    printf("x\t\txs\t\txsi\t\tssi\t\txi\n");
    for (int x = 0; x < 10; x++) {
        xs = (double)x / 10.0 * 0.366025403784438646763729;
        int xsb = (int)(xs < (int)xs ? (int)xs - 1 : (int)xs);
        xsi = xs - xsb;
        ssi = (xsi + xsi) * -0.211324865405187023657952;
        xi = xsi + ssi;
        printf("%d\t%.6f\t%.6f\t%.6f\t%.6f\n", x, xs, xsi, ssi, xi);
    }
    
    free(ctx);
    return 0;
}
