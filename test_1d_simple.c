/*
 * Test file for OpenSimplex2F_noise1 and OpenSimplex2F_noise1_fast functions.
 * This file tests that both functions produce identical results.
 */

#include <stdio.h>
#include <math.h>
#include "OpenSimplex2F.h"

int main(void)
{
	struct OpenSimplex2F_context *ctx;
	int64_t seed = 123456789ULL;
	int tests_passed = 1;

	// Initialize context
	if (OpenSimplex2F(seed, &ctx) != 0) {
		printf("Failed to initialize OpenSimplex2F context\n");
		return 1;
	}

	// Test points
	double test_points[] = {0.0, 0.5, 1.0, -0.5, 5.5, -5.5};
	int num_tests = sizeof(test_points) / sizeof(test_points[0]);

	printf("Testing OpenSimplex2F_noise1...\n");
	for (int i = 0; i < num_tests; i++) {
		double x = test_points[i];
		double result = OpenSimplex2F_noise1(ctx, x);
		printf("noise1(%.1f) = %.6f\n", x, result);
	}

	printf("\nTesting OpenSimplex2F_noise1_fast...\n");
	for (int i = 0; i < num_tests; i++) {
		double x = test_points[i];
		double result = OpenSimplex2F_noise1_fast(ctx, x);
		printf("noise1_fast(%.1f) = %.6f\n", x, result);
	}

	// Verify that both functions produce identical results
	printf("\nVerifying identical results...\n");
	for (int i = 0; i < num_tests; i++) {
		double x = test_points[i];
		double result1 = OpenSimplex2F_noise1(ctx, x);
		double result_fast = OpenSimplex2F_noise1_fast(ctx, x);

		if (fabs(result1 - result_fast) > 1e-10) {
			printf("MISMATCH at x = %.1f: noise1=%.6f, noise1_fast=%.6f\n", x, result1, result_fast);
			tests_passed = 0;
		}
	}

	if (tests_passed) {
		printf("\nAll tests passed!\n");
	} else {
		printf("\nSome tests failed!\n");
	}

	// Cleanup
	OpenSimplex2F_free(ctx);
	return tests_passed ? 0 : 1;
}
