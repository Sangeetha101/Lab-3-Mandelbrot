#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#include "pngwriter.h"
#include "consts.h"

unsigned long get_time()
{
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return tp.tv_sec * 1000000 + tp.tv_usec;
}

int main(int argc, char** argv)
{
    png_data* pPng = png_create(IMAGE_WIDTH, IMAGE_HEIGHT);

    double a, b, a2, b2, ka, kb;
    kb = MIN_Y;

    double fDeltaX = (MAX_X - MIN_X) / (double)IMAGE_WIDTH;
    double fDeltaY = (MAX_Y - MIN_Y) / (double)IMAGE_HEIGHT;

    long nTotalIterationsCount = 0;
    unsigned long nTimeStart = get_time();

    long i, j, n;

    n = 0;
    int k;

#pragma omp parallel
    {
#pragma omp for private(i, k) reduction(+ : ka, kb)
        for (j = 0; j < IMAGE_HEIGHT; j++) {
            ka = MIN_X;

            for (i = 0; i < IMAGE_WIDTH; i++) {
                a = ka;
                b = kb;

                a2 = a * a;
                b2 = b * b;

                for (n = 0; (n < MAX_ITERS) && (a2 + b2 < 4); n++) {
                    b = 2 * a * b + kb;
                    a = a2 - b2 + ka;
                    a2 = a * a;
                    b2 = b * b;
                }

                int k = ((long)n * 255) / MAX_ITERS;
                png_plot(pPng, i, j, k, k, k);

                ka += fDeltaX;

                nTotalIterationsCount++;
            }

            kb += fDeltaY;
        }
    }

    unsigned long nTimeEnd = get_time();

    png_write(pPng, "mandel.png");
    return 0;
}
