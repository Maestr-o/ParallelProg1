#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int coefficient;
    int *exponents;
} mon_t;

int *fill_array(FILE *fp, int *k);
void mult_pol(int n, mon_t pol1[], int size1, mon_t pol2[], int size2, mon_t res[]);
void fill_struct(mon_t *s, int *p, int n, int k);
void print(mon_t res[], int n, int k);
void free_pol(mon_t res[], int k);
int check_file(FILE *fp);
void run();

int main() {
    run();
    return 0;
}

void run() {
    char path[256];
    printf("Enter the path to file: ");
    scanf("%255s", path);
    FILE *fp = fopen(path, "r");
    if (!check_file(fp)) exit(1);
    int n = 0, k1 = 0, k2 = 0;
    char c = getc(fp);
    while (c != '\n') {
        n = n * 10 + c - '0';
        c = getc(fp);
    }
    int *p1 = fill_array(fp, &k1);
    int *p2 = fill_array(fp, &k2);
    fclose(fp);
    k1 /= (n + 1);
    k2 /= (n + 1);
    mon_t pol1[k1], pol2[k2], res[k1 * k2];
    fill_struct(pol1, p1, n, k1);
    fill_struct(pol2, p2, n, k2);
    mult_pol(n, pol1, k1, pol2, k2, res);
    //очистка
    free(p1);
    free(p2);
    free_pol(pol1, k1);
    free_pol(pol2, k2);
    free_pol(res, k1 * k2);
}

int *fill_array(FILE *fp, int *k) {
    int *p = (int *)malloc(sizeof(int));
    int i = 0;
    char c = getc(fp);
    while (c != EOF && c != '\n') {
        int sign = 1;
        if (c == '-') {
            sign = -1;
            c = getc(fp);
        }
        int num = c - '0';
        p = (int *)realloc(p, sizeof(int) * (i + 2));
        c = getc(fp);
        while (c >= '0' && c <= '9' && c != '\n' && c != EOF) {
            num = num * 10 + c - '0';
            c = getc(fp);
        }
        p[i++] = num * sign;
        if (c != '\n') c = getc(fp);
    }
    *k = i;
    return p;
}

void mult_pol(int n, mon_t pol1[], int k1, mon_t pol2[], int k2, mon_t res[]) {
    int size_result = 0;
    for (int i = 0; i < k1; i++) {
        for (int j = 0; j < k2; j++) {
            res[size_result].coefficient = pol1[i].coefficient * pol2[j].coefficient;
            res[size_result].exponents = (int *)calloc(sizeof(int), n);
            for (int k = 0; k < n; k++) {
                res[size_result].exponents[k] = pol1[i].exponents[k] + pol2[j].exponents[k];
            }
            size_result++;
        }
    }
    size_result = 0;
    for (int i = 0; i < k1 * k2 - 1; i++) { // подсчет подобных
        for (int j = i + 1; j < k1 * k2 && res[i].coefficient != 0; j++) {
            if (res[j].coefficient != 0) {
                if (res[i].exponents[0] == res[j].exponents[0] &&
                    res[i].exponents[1] == res[j].exponents[1] &&
                    res[i].exponents[2] == res[j].exponents[2]) {
                    res[i].coefficient += res[j].coefficient;
                    res[j].coefficient = 0;
                    size_result++;
                }
            }
        }
    }
}

void fill_struct(mon_t *s, int *p, int n, int k) {
    for (int i = 0; i < k; i++) {
        s[i].exponents = (int *)malloc(sizeof(int) * n);
        s[i].coefficient = p[i * (n + 1)];
        for (int j = 0; j < n; j++) {
            s[i].exponents[j] = p[i * (n + 1) + j + 1];
        }
    }
}

void print(mon_t res[], int n, int k) {
    for (int i = 0; i < k; i++) {
        if (res[i].coefficient != 0) {
            printf("(%d, ", res[i].coefficient);
            for (int j = 0; j < n - 1; j++) {
                printf("%d, ", res[i].exponents[j]);
            }
            printf("%d); ", res[i].exponents[n - 1]);
        }
    }
    printf("\n");
}

void free_pol(mon_t res[], int k) {
    for (int i = 0; i < k; i++) {
        free(res[i].exponents);
    }
}

int check_file(FILE *fp) {
    if (!fp) {
        printf("Can't open the file");
        return 0;
    }
    return 1;
}