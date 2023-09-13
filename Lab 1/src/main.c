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
    int k = 0;
    int *p1 = fill_pol(fp, &k);
    int *p2 = fill_pol(fp, &k);
    fclose(fp);
    print_pol(p1, k);
    print_pol(p2, k);
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
        int num = c - '0';
        p = (int *)realloc(p, sizeof(int) * (i + 2));
        c = getc(fp);
        while (c >= '0' && c <= '9' && c != '\n' && c != EOF) {
            num = num * 10 + c - '0';
            c = getc(fp);
        }
        p[i++] = num;
        if (c != '\n') c = getc(fp);
    }
    *k = i;
    return p;
}