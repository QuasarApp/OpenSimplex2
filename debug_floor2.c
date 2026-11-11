#include <stdio.h>
#include <math.h>

static int fastFloor(double x) {
    int xi = (int)x;
    return x < xi ? xi - 1 : xi;
}

int main(void) {
    for (int x = 0; x < 50; x++) {
        double xs = (double)x * 0.366025403784438646763729;
        int xsb = fastFloor(xs);
        double xsi = xs - xsb;
        printf("x=%d xs=%.6f xsb=%d xsi=%.6f\n", x, xs, xsb, xsi);
    }
    return 0;
}
