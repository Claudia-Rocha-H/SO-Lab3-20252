/*
 * fibonacci.c
 * Generador de la secuencia de Fibonacci usando un hilo trabajador
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
    long long *arr;
    int n;
} fib_arg_t;

void *worker(void *arg)
{
    fib_arg_t *a = (fib_arg_t *) arg;
    long long *arr = a->arr;
    int n = a->n;

    if (n <= 0) return NULL;
    if (n >= 1) arr[0] = 0;
    if (n >= 2) arr[1] = 1;
    for (int i = 2; i < n; ++i) {
        arr[i] = arr[i-1] + arr[i-2];
    }
    return NULL;
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s N\n", argv[0]);
        return 1;
    }
    int N = atoi(argv[1]);
    if (N <= 0) {
        fprintf(stderr, "N must be > 0\n");
        return 1;
    }

    long long *arr = malloc(sizeof(long long) * (size_t)N);
    if (!arr) {
        perror("malloc");
        return 1;
    }

    pthread_t th;
    fib_arg_t arg;
    arg.arr = arr;
    arg.n = N;

    int rc = pthread_create(&th, NULL, worker, &arg);
    if (rc != 0) {
        fprintf(stderr, "pthread_create failed (%d)\n", rc);
        free(arr);
        return 1;
    }

    rc = pthread_join(th, NULL);
    if (rc != 0) {
        fprintf(stderr, "pthread_join failed (%d)\n", rc);
        free(arr);
        return 1;
    }

    for (int i = 0; i < N; ++i) {
        printf("%d: %lld\n", i, arr[i]);
    }

    free(arr);
    return 0;
}
