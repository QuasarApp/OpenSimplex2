#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

static const double F2_1D = 0.366025403784438646763729;
static const double G2_1D = 0.211324865405187023657952;

static int fastFloor_1D(double x) {
    int xi = (int)x;
    printf("fastFloor_1D: x=%.15f xi=%d x<xi=%d\n", x, xi, x < xi);
    return x < xi ? xi - 1 : xi;
}

int main(void) {
    printf("Testing xsb calculation with F2_1D:\n");
    
    for (int x = 0; x <= 10; x++) {
        double pos = (double)x / 10.0;
        double xs = pos * F2_1D;
        printf("x=%d pos=%.15f xs=%.15f ", x, pos, xs);
        int xsb = fastFloor_1D(xs);
        double xsi = xs - xsb;
        printf("xsb=%d xsi=%.15f\n", xsb, xsi);
    }
    return 0;
}
