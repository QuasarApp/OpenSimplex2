#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static int fastFloor(double x) {
    int xi = (int)x;
    printf("fastFloor: x=%.6f xi=%d x<xi=%d\n", x, xi, x < xi);
    return x < xi ? xi - 1 : xi;
}

int main(void) {
    double F2 = 0.366025403784438646763729;
    for (int x = 0; x < 10; x++) {
        double xs = x * F2;
        int xsb = fastFloor(xs);
        double xsi = xs - xsb;
        printf("x=%d xs=%.6f xsb=%d xsi=%.6f\n", x, xs, xsb, xsi);
    }
    return 0;
}
