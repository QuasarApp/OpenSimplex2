#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static int fastFloor(double x) {
    int xi = (int)x;
    printf("fastFloor: x=%.15f xi=%d x<xi=%d\n", x, xi, x < xi);
    return x < xi ? xi - 1 : xi;
}

int main(void) {
    printf("Testing xsb calculation:\n");
    double F2 = 0.366025403784438646763729;
    
    for (int x = 0; x <= 10; x++) {
        double pos = (double)x / 10.0;
        double xs = pos * F2;
        printf("x=%d pos=%.15f xs=%.15f ", x, pos, xs);
        int xsb = fastFloor(xs);
        double xsi = xs - xsb;
        printf("xsb=%d xsi=%.15f\n", xsb, xsi);
    }
    return 0;
}
