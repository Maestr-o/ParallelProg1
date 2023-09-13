#include <stdio.h>
#include <stdlib.h>

void print_pol(int *p, int k);
int *fill_pol(FILE *fp, int *k);

int main() {
    char path[256];
    printf("Enter the path to file: ");
    scanf("%255s", path);
    FILE *fp = fopen(path, "r");
    if (!fp) {
        printf("Can't open the file");
        return 0;
    }
    int k1 = 0, k2 = 0;
    int *p1 = fill_pol(fp, &k1);
    int *p2 = fill_pol(fp, &k2);
    fclose(fp);
    print_pol(p1, k1);
    print_pol(p2, k2);
    free(p1);
    free(p2);
    return 0;
}

void print_pol(int *p, int k) {
    for (int i = 0; i < k; i++)
        printf("%d ", p[i]);
    printf("\n");
}

int *fill_pol(FILE *fp, int *k) {
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