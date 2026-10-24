#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define LOW 11020.0f
#define HIGH 31702.0f
#define SEED 12

typedef void (*Sort)(float *, int);

void insertion(float *a, int n)
{
    for (int i = 1; i < n; i++) {
        float x = a[i];
        int j = i - 1;
        while (j >= 0 && a[j] > x) {
            a[j + 1] = a[j];
            j--;
        }
        a[j + 1] = x;
    }
}

/* Разбиение Хоара: опорным выбирается средний элемент. */
int partition(float *a, int left, int right)
{
    float pivot = a[left + (right - left) / 2];
    int i = left - 1, j = right + 1;
    for (;;) {
        do { i++; } while (a[i] < pivot);
        do { j--; } while (a[j] > pivot);
        if (i >= j) return j;
        float t = a[i]; a[i] = a[j]; a[j] = t;
    }
}

void hoare(float *a, int left, int right)
{
    if (left >= right) return;
    int p = partition(a, left, right);
    hoare(a, left, p);
    hoare(a, p + 1, right);
}

void recursive_sort(float *a, int n)
{
    hoare(a, 0, n - 1);
}

void iterative_sort(float *a, int n)
{
    /* Откладываем больший отрезок, обрабатываем меньший.
       Поэтому для int-размеров достаточно 64 пар границ. */
    int ls[64], rs[64], top = 0;
    int left = 0, right = n - 1;
    for (;;) {
        while (left < right) {
            int p = partition(a, left, right);
            if (p - left < right - p - 1) {
                ls[top] = p + 1; rs[top++] = right;
                right = p;
            } else {
                ls[top] = left; rs[top++] = p;
                left = p + 1;
            }
        }
        if (top == 0) break;
        left = ls[--top]; right = rs[top];
    }
}

void fill(float *a, int n)
{
    for (int i = 0; i < n; i++)
        a[i] = LOW + (HIGH - LOW) * ((float)rand() / RAND_MAX);
}

void print_array(const float *a, int n)
{
    for (int i = 0; i < n; i++)
        printf("%9.2f%s", a[i], (i + 1) % 5 == 0 ? "\n" : " ");
    if (n % 5) putchar('\n');
}

int compare(const void *a, const void *b)
{
    float x = *(const float *)a, y = *(const float *)b;
    return (x > y) - (x < y);
}

int equal(const float *a, const float *b, int n)
{
    for (int i = 0; i < n; i++)
        if (a[i] != b[i]) return 0;
    return 1;
}

double now_ms(void)
{
    struct timespec t;
    if (clock_gettime(CLOCK_MONOTONIC, &t) != 0) {
        perror("clock_gettime"); exit(EXIT_FAILURE);
    }
    return t.tv_sec * 1000.0 + t.tv_nsec / 1000000.0;
}

int main(void)
{
    Sort sorts[] = {iterative_sort, recursive_sort, insertion};
    const char *names[] = {"Быстрая без рекурсии", "Быстрая Хоара",
                           "Вставками"};
    int sizes[] = {1000, 10000, 100000, 500000, 1000000};
    setvbuf(stdout, NULL, _IOLBF, 0);
    srand(SEED);
    printf("Вариант 12. float. Диапазон [%.0f; %.0f]. seed=%d\n",
           LOW, HIGH, SEED);
    float source[50], work[50], reference[50];
    fill(source, 50);
    memcpy(reference, source, sizeof source);
    qsort(reference, 50, sizeof(float), compare);
    puts("\nИсходный массив, N=50:"); print_array(source, 50);
    for (int k = 0; k < 3; k++) {
        memcpy(work, source, sizeof source);
        sorts[k](work, 50);
        printf("\n%s:\n", names[k]); print_array(work, 50);
        if (!equal(work, reference, 50)) return EXIT_FAILURE;
        puts("Совпадение с qsort: OK");
    }
    FILE *csv = fopen("results.csv", "w");
    if (!csv) { perror("results.csv"); return EXIT_FAILURE; }
    fprintf(csv, "N,iterative_ms,hoare_ms,insertion_ms\n");
    puts("\nN; без рекурсии, мс; Хоара, мс; вставками, мс");
    for (int s = 0; s < 5; s++) {
        int n = sizes[s];
        float *a = malloc(n * sizeof(float));
        float *b = malloc(n * sizeof(float));
        float *ref = malloc(n * sizeof(float));
        if (!a || !b || !ref) {
            free(a); free(b); free(ref); fclose(csv);
            fputs("Недостаточно памяти\n", stderr); return EXIT_FAILURE;
        }
        srand(SEED); fill(a, n);
        memcpy(ref, a, n * sizeof(float));
        qsort(ref, n, sizeof(float), compare);
        double times[3];
        for (int k = 0; k < 3; k++) {
            memcpy(b, a, n * sizeof(float));
            double start = now_ms();
            sorts[k](b, n);
            times[k] = now_ms() - start;
            if (!equal(b, ref, n)) {
                fputs("Ошибка сортировки\n", stderr);
                free(a); free(b); free(ref); fclose(csv);
                return EXIT_FAILURE;
            }
        }
        printf("%d; %.3f; %.3f; %.3f; OK\n",
               n, times[0], times[1], times[2]);
        fprintf(csv, "%d,%.3f,%.3f,%.3f\n",
                n, times[0], times[1], times[2]);
        fflush(csv);
        free(a); free(b); free(ref);
    }
    fclose(csv);
    return EXIT_SUCCESS;
}
