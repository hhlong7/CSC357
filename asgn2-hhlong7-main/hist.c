#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hist.h"

void print_histogram(int *points, int n) {
    int i, x, y;
    int min_val, max_val;
    int xmin, xmax, ymax;
    int num_bars, max_count;
    int *counts;

    find_min_max(points, n, &min_val, &max_val);
    xmin = lower(min_val);
    xmax = upper(max_val);
    num_bars = xmax - xmin + 1;

    counts = build_counts(points, n, xmin, num_bars);

    max_count = 0;
    for (i = 0; i < num_bars; i++)
        if (counts[i] > max_count) max_count = counts[i];
    ymax = upper(max_count);

    /* top border row */
    printf("    |");
    for (i = 0; i < num_bars + 2; i++) printf(" ");
    printf("|\n");

    /* rows from ymax down to 1 */
    for (y = ymax; y >= 1; y--) {
        if (y % 5 == 0)
            printf("%3d T", y);
        else
            printf("    |");

        printf(" ");
        for (x = xmin; x <= xmax; x++)
            printf("%c", counts[x - xmin] >= y ? '#' : ' ');
        printf(" ");

        if (y % 5 == 0)
            printf("T %d\n", y);
        else
            printf("|\n");
    }

    /* bottom border */
    printf("    +-");
    for (x = xmin; x <= xmax; x++)
        printf("%c", x % 5 == 0 ? '|' : '-');
    printf("-+\n");

    print_x_labels(xmin, xmax);

    free(counts);
}

int main(int argc, char *argv[]) {
    char line[MAX_LINE];
    int points[MAX_ROWS];
    int nrows = 0;
    int ncols = 0;
    int sum;
    int result;
    (void)argc;

    fgets(line, MAX_LINE, stdin);
    ncols = count_columns(line);

    while (fgets(line, MAX_LINE, stdin) != NULL) {
        result = row(line, ncols, &sum);
        if (result == 1) {
            printf("%s: Mismatched cells\n", argv[0]);
            return 1;
        }
        if (result == 2) {
            printf("%s: Unsupported quotes\n", argv[0]);
            return 2;
        }
        points[nrows++] = sum;
    }

    print_histogram(points, nrows);
    return 0;
}
