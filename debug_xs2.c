#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "OpenSimplex2F.h"
#include "OpenSimplex2F_1D.h"

static int fastFloor_1D(double x) {
    int xi = (int)x;
    printf("fastFloor_1D: x=%.15f xi=%d x<xi=%d\n", x, xi, x < xi);
    return x < xi ? xi - 1 : xi;
}

static int fastFloor(double x) {
    int xi = (int)x;
    printf("fastFloor: x=%.15f xi=%d x<xi=%d\n", x, xi, x < xi);
    return x < xi ? xi - 1 : xi;
}

int main(void) {
    struct OpenSimplex2F_context *ctx;
    int seed = 42;
    
    if (OpenSimplex2F_1D(seed, &ctx) != 0) {
        fprintf(stderr, "Failed to initialize\n");
        return 1;
    }
    
    printf("x\tpos\txs\txsb (fastFloor_1D)\txsi (fastFloor_1D)\n");
    for (int x = 0; x < 10; x++) {
        double pos = (double)x / 10.0;
        double xs = pos * 0.366025403784438646763729;
        int xsb1 = fastFloor_1D(xs);
        double xsi1 = xs - xsb1;
        
        int xsb2 = fastFloor(xs);
        double xsi2 = xs - xsb2;
        
        printf("%d\t%.15f\t%.15f\t%d\t%.15f\t%d\t%.15f\n",
               x, pos, xs, xsb1, xsi1, xsb2, xsi2);
    }
    
    free(ctx);
    return 0;
}
