#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hist.h"

/* Rounds it to the highest in range mult of 5 */
int upper(int n) {
    if (n % 5 == 0) return n;
    if (n > 0) return (n / 5 + 1) * 5;
    return (n / 5) * 5;
}

/* Rounds it to the lowest in range mult of 5*/
int lower(int n) {
    if (n % 5 == 0) return n;
    if (n > 0) return (n / 5) * 5;
    return (n / 5 - 1) * 5;
}

int count_columns(const char *line) {
    int count = 1;
    int i;
    for (i = 0; line[i] != '\0' && line[i] != '\n'; i++) {
        if (line[i] == ',') count++;
    }
    return count;
}

void strip_whitespace(char *cell) {
    int i, j = 0;
    for (i = 0; cell[i] != '\0'; i++) {
        if (cell[i] != ' ' && cell[i] != '\t') {
            cell[j++] = cell[i];
        }
    }
    cell[j] = '\0';
}

int cell_to_int(const char *cell, int *out) {
    char buf[2048];
    char *end;
    long val;
    strncpy(buf, cell, 2047);
    buf[2047] = '\0';
    strip_whitespace(buf);
    if (buf[0] == '\0') return 0;
    val = strtol(buf, &end, 10);
    if (*end != '\0') return 0;
    *out = (int)val;
    return 1;
}

int row(const char *line, int expected_cols, int *sum_out) {
    char cell[2048];
    int ncells = 0;
    int sum = 0;
    int i = 0, j = 0;
    int val;

    if (strchr(line, '"') != NULL || strchr(line, '\'') != NULL) {
        return 2;
    }

    while (line[i] != '\0' && line[i] != '\n') {
        if (line[i] == ',') {
            cell[j] = '\0';
            ncells++;
            if (cell_to_int(cell, &val)) sum += val;
            j = 0;
        } else {
            cell[j++] = line[i];
        }
        i++;
    }
    cell[j] = '\0';
    ncells++;
    if (cell_to_int(cell, &val)) sum += val;

    if (ncells != expected_cols) return 1;
    *sum_out = sum;
    return 0;
}

void find_min_max(int *points, int n, int *min, int *max) {
    int i;
    *min = points[0];
    *max = points[0];
    for (i = 1; i < n; i++) {
        if (points[i] < *min) *min = points[i];
        if (points[i] > *max) *max = points[i];
    }
}

int *build_counts(int *points, int n, int xmin, int num_bars) {
    int i;
    int *counts = (int *)calloc(num_bars, sizeof(int));
    for (i = 0; i < n; i++) {
        counts[points[i] - xmin]++;
    }
    return counts;
}

void print_x_labels(int xmin, int xmax) {
    int x, i, pos, label_len;
    int num_bars = xmax - xmin + 1;
    int total_len = 6 + num_bars;
    char label[16];
    char x_axis[MAX_LINE];

    for (i = 0; i < total_len; i++) x_axis[i] = ' ';
    x_axis[total_len] = '\0';

    for (x = xmin; x <= xmax; x++) {
        if (x % 5 == 0) {
            sprintf(label, "%d", x);
            label_len = (int)strlen(label);
            pos = 6 + (x - xmin);
            for (i = label_len - 1; i >= 0; i--)
                x_axis[pos--] = label[i];
        }
    }
    printf("%s\n", x_axis);
}
