#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define SEQ_OUT_PATH "../build/sout.txt"
#define PAR_OUT_PATH "../build/pout.txt"

typedef struct { // структура монома
    long coefficient;
    long *exponents;
} mon_t;

long *fill_array(FILE *fp, int *k); // заполнение массива-полинома из файла
void mult_pol(int n, mon_t pol1[], int size1, mon_t pol2[], int size2, mon_t res[]); // последовательное перемножение
void mult_pol_parallel(int n, mon_t pol1[], int k1, mon_t pol2[], int k2, mon_t res[]); // параллельное перемножение
void fill_struct(mon_t *s, long *p, int n, int k); // заполнение массива структур
void print(mon_t res[], int n, int k); // вывод результата в консоль
void print_to_file(mon_t res[], int n, int k, char *path); // вывод результата в файл
void free_pol(mon_t res[], int k); // освобождение памяти массива структур
int compare(mon_t res1[], mon_t res2[], int n, int k); // сравнение
int check_file(FILE *fp); //проверка на существование файла
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
    if (!check_file(fp)) exit(0);
    int n = 0, k1 = 0, k2 = 0;

    char c = getc(fp); // получение кол-ва переменных
    while (c != '\n') {
        n = n * 10 + c - '0';
        c = getc(fp);
    }

    long *p1 = fill_array(fp, &k1); // заполнение массивов
    long *p2 = fill_array(fp, &k2);
    fclose(fp);
    k1 /= (n + 1);
    k2 /= (n + 1);
    mon_t pol1[k1], pol2[k2], res[k1 * k2], res_par[k1 * k2];

    fill_struct(pol1, p1, n, k1); // заполнение структур
    fill_struct(pol2, p2, n, k2);

    double start, end; // вычисление
    start = omp_get_wtime();
    mult_pol(n, pol1, k1, pol2, k2, res);
    end = omp_get_wtime();
    printf("Seq: %.3lf sec\n", (end - start));
    start = omp_get_wtime();
    mult_pol_parallel(n, pol1, k1, pol2, k2, res_par);
    end = omp_get_wtime();
    printf("Parallel: %.3lf sec\n", (end - start));

    if (compare(res, res_par, n, k1 * k2) == 0) // сравнение
        printf("Results are same!\n");
    else {
        printf("Results aren't same!\n");
        return;
    }

    if (k1 > 10 || k2 > 10) { // вывод результата
        print_to_file(res, n, k1 * k2, SEQ_OUT_PATH);
        print_to_file(res_par, n, k1 * k2, PAR_OUT_PATH);
    } else {
        printf("Seq:\n");
        print(res, n, k1 * k2);
        printf("\nParallel:\n");
        print(res_par, n, k1 * k2);
    }

    free(p1); // освобождение памяти
    free(p2);
    free_pol(pol1, k1);
    free_pol(pol2, k2);
    free_pol(res, k1 * k2);
    free_pol(res_par, k1 * k2);
}

long *fill_array(FILE *fp, int *k) {
    long *p = (long *)malloc(sizeof(long));
    int i = 0;
    char c = getc(fp);
    while (c != EOF && c != '\n') {
        int sign = 1;
        if (c == '-') {
            sign = -1;
            c = getc(fp);
        }
        int num = c - '0';
        p = (long *)realloc(p, sizeof(long) * (i + 2));
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
            res[size_result].exponents = (long *)calloc(sizeof(long), n);
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

void mult_pol_parallel(int n, mon_t pol1[], int k1, mon_t pol2[], int k2, mon_t res[]) {
    #pragma omp parallel for
    for (int i = 0; i < k1; i++) {
        for (int j = 0; j < k2; j++) {
            int local_size_result = i * k2 + j;
            res[local_size_result].coefficient = pol1[i].coefficient * pol2[j].coefficient;
            res[local_size_result].exponents = (long *)calloc(sizeof(long), n);
            for (int k = 0; k < n; k++) {
                res[local_size_result].exponents[k] = pol1[i].exponents[k] + pol2[j].exponents[k];
            }
        }
    }
    int size_result = 0;
    #pragma omp parallel for reduction(+:size_result)
    for (int i = 0; i < k1 * k2 - 1; i++) { // подсчет подобных
        for (int j = i + 1; j < k1 * k2 && res[i].coefficient != 0; j++) {
            if (res[j].coefficient != 0) {
                if (res[i].exponents[0] == res[j].exponents[0] &&
                    res[i].exponents[1] == res[j].exponents[1] &&
                    res[i].exponents[2] == res[j].exponents[2]) {
                    #pragma omp atomic
                    res[i].coefficient += res[j].coefficient;
                    res[j].coefficient = 0;
                    #pragma omp atomic
                    size_result++;
                }
            }
        }
    }
}

void fill_struct(mon_t *s, long *p, int n, int k) {
    for (int i = 0; i < k; i++) {
        s[i].exponents = (long *)malloc(sizeof(long) * n);
        s[i].coefficient = p[i * (n + 1)];
        for (int j = 0; j < n; j++) {
            s[i].exponents[j] = p[i * (n + 1) + j + 1];
        }
    }
}

void print(mon_t res[], int n, int k) {
    for (int i = 0; i < k; i++) {
        if (res[i].coefficient != 0) {
            printf("(%ld, ", res[i].coefficient);
            for (int j = 0; j < n - 1; j++) {
                printf("%ld, ", res[i].exponents[j]);
            }
            printf("%ld); ", res[i].exponents[n - 1]);
        }
    }
}

void print_to_file(mon_t res[], int n, int k, char *path) {
    FILE *fp = fopen(path, "w");
    if (!check_file(fp)) return;
    for (int i = 0; i < k; i++) {
        if (res[i].coefficient != 0) {
            fprintf(fp, "(%ld, ", res[i].coefficient);
            for (int j = 0; j < n - 1; j++) {
                fprintf(fp, "%ld, ", res[i].exponents[j]);
            }
            fprintf(fp, "%ld); ", res[i].exponents[n - 1]);
        }
    }
    fclose(fp);
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

int compare(mon_t res1[], mon_t res2[], int n, int k) {
    for (int i = 0; i < k; i++) {
        if (res1[i].coefficient == res2[i].coefficient) {
            for (int j = 0; j < n; j++) {
                if (res1[i].exponents[j] != res2[i].exponents[j])
                    return 1;
            }
        } else
            return 1;
    }
    return 0;
}