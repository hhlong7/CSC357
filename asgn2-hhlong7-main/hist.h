#define MAX_LINE 2048
#define MAX_ROWS 1024

int count_columns(const char *line);

void strip_whitespace(char *cell);

int cell_to_int(const char *cell, int *out);

int row(const char *line, int expected_cols, int *sum_out);

void print_histogram(int *points, int n);

int lower(int n);

int upper(int n);

void find_min_max(int *points, int n, int *min, int *max);

int *build_counts(int *points, int n, int xmin, int num_bars);

void print_x_labels(int xmin, int xmax);
