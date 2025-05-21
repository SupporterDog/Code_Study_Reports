#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bmplib.h"
#include "hw2.h"

void filter_optimized(void* args[]) {
    unsigned int width = *(unsigned int*)args[0];
    unsigned int height = *(unsigned int*)args[1];
    Pixel* input = (Pixel*)args[2];
    Pixel* output = (Pixel*)args[3];
    float* filter = (float*)args[4];

    for (int y = 0; y < height; ++y) {
        Pixel* row_output = output + y * width;
        for (int x = 0; x < width; ++x) {
            double r = 0;
            double g = 0;
            double b = 0;

            int x0 = x - 1;
            int x1 = x;
            int x2 = x + 1;

            int y0 = y - 1;
            int y1 = y;
            int y2 = y + 1;

            int x0_valid = (x0 >= 0);
            int x2_valid = (x2 < width);
            int y0_valid = (y0 >= 0);
            int y2_valid = (y2 < height);

            Pixel* row0 = y0_valid ? &input[y0 * width] : NULL;
            Pixel* row1 = &input[y1 * width];
            Pixel* row2 = y2_valid ? &input[y2 * width] : NULL;

            if (x0_valid && y0_valid) {
                r += row0[x0].r * filter[0];
                g += row0[x0].g * filter[0];
                b += row0[x0].b * filter[0];
            }
            if (y0_valid) {
                r += row0[x1].r * filter[1];
                g += row0[x1].g * filter[1];
                b += row0[x1].b * filter[1];
            }
            if (x2_valid && y0_valid) {
                r += row0[x2].r * filter[2];
                g += row0[x2].g * filter[2];
                b += row0[x2].b * filter[2];
            }
            if (x0_valid) {
                r += row1[x0].r * filter[3];
                g += row1[x0].g * filter[3];
                b += row1[x0].b * filter[3];
            }
            r += row1[x1].r * filter[4];
            g += row1[x1].g * filter[4];
            b += row1[x1].b * filter[4];
            if (x2_valid) {
                r += row1[x2].r * filter[5];
                g += row1[x2].g * filter[5];
                b += row1[x2].b * filter[5];
            }
            if (x0_valid && y2_valid) {
                r += row2[x0].r * filter[6];
                g += row2[x0].g * filter[6];
                b += row2[x0].b * filter[6];
            }
            if (y2_valid) {
                r += row2[x1].r * filter[7];
                g += row2[x1].g * filter[7];
                b += row2[x1].b * filter[7];
            }
            if (x2_valid && y2_valid) {
                r += row2[x2].r * filter[8];
                g += row2[x2].g * filter[8];
                b += row2[x2].b * filter[8];
            }

            Pixel p;
            p.r = (unsigned char)(r < 0 ? 0 : (r > 255 ? 255 : r));
            p.g = (unsigned char)(g < 0 ? 0 : (g > 255 ? 255 : g));
            p.b = (unsigned char)(b < 0 ? 0 : (b > 255 ? 255 : b));

            row_output[x] = p;
        }
    }
}