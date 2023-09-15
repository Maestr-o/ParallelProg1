#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int coefficient;
    int *exponents;
} term_t;

void print_pol(term_t pol[], int n, int k);
int *fill_array(FILE *fp, int *k);
void mult_pol(int n, term_t pol1[], int size1, term_t pol2[], int size2, term_t result[]);
void fill_struct(term_t *s, int *p, int n, int k);

int main() {
    char path[256];
    printf("Enter the path to file: ");
    //scanf("%255s", path);
    strcpy(path, "../datasets/1.txt");
    FILE *fp = fopen(path, "r");
    if (!fp) {
        printf("Can't open the file");
        return 0;
    }
    int n = 0, k1 = 0, k2 = 0;
    char c = getc(fp);
    while (c != '\n') {
        n = n * 10 + c - '0';
        c = getc(fp);
    }
    int *p1 = fill_array(fp, &k1);
    int *p2 = fill_array(fp, &k2);
    k1 /= (n + 1);
    k2 /= (n + 1);
    term_t pol1[k1], pol2[k2], res[k1 + k2];
    fill_struct(pol1, p1, n, k1);
    fill_struct(pol2, p2, n, k2);
    mult_pol(n, pol1, k1, pol2, k2, res);

    for (int i = 0; i < k1 * k2; i++) { // вывод
        printf("(%d, ", res[i].coefficient);
        for (int j = 0; j < n - 1; j++) {
            printf("%d, ", res[i].exponents[j]);
        }
        printf("%d); ", res[i].exponents[n - 1]);
    }
    printf("\n");
    //free
    return 0;
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

void mult_pol(int n, term_t pol1[], int size1, term_t pol2[], int size2, term_t result[]) {
    int i, j, k;
    int size_result = 0;
    for (i = 0; i < size1; i++) {
        for (j = 0; j < size2; j++) {
            result[size_result].coefficient = pol1[i].coefficient * pol2[j].coefficient;
            result[size_result].exponents = (int *)malloc(sizeof(int) * n);
            for (k = 0; k < n; k++) {
                result[size_result].exponents[k] = pol1[i].exponents[k] + pol2[j].exponents[k];
            }
            size_result++;
        }
    }
}

void fill_struct(term_t *s, int *p, int n, int k) {
    for (int i = 0; i < k; i++) {
        s[i].exponents = (int *)malloc(sizeof(int) * n);
        s[i].coefficient = p[i * (n + 1)];
        for (int j = 0; j < n; j++) {
            s[i].exponents[j] = p[i * (n + 1) + j + 1];
        }
    }
}