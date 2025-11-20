/*
 * Paralelización del cálculo de Pi usando Pthreads
 */

/* Enable POSIX feature test macros so CLOCK_MONOTONIC is defined */
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199309L
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <time.h>
#include <assert.h>

typedef struct {
    long start;
    long end; /* exclusive */
    double fH;
} thread_arg_t;

static double now_sec(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

void *thread_calc(void *arg)
{
    thread_arg_t *a = (thread_arg_t *) arg;
    double sum = 0.0;
    for (long i = a->start; i < a->end; ++i) {
        double x = a->fH * ((double)i + 0.5);
        sum += 4.0 / (1.0 + x * x);
    }
    double *ret = malloc(sizeof(double));
    if (!ret) return NULL;
    *ret = sum;
    return ret;
}

int main(int argc, char **argv)
{
    long n = 2000000000L; 
    int T = 0;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <num_threads> [n_intervals]\n", argv[0]);
        fprintf(stderr, "  Example: %s 4 2000000000\n", argv[0]);
        return 1;
    }

    T = atoi(argv[1]);
    if (T <= 0) {
        fprintf(stderr, "num_threads must be > 0\n");
        return 1;
    }

    if (argc >= 3) {
        n = atol(argv[2]);
    }

    if (n <= 0) {
        fprintf(stderr, "n must be > 0\n");
        return 1;
    }

    const double fH = 1.0 / (double) n;

    pthread_t *threads = malloc(sizeof(pthread_t) * T);
    thread_arg_t *args = malloc(sizeof(thread_arg_t) * T);
    if (!threads || !args) {
        perror("malloc");
        return 1;
    }

    long base = n / T;
    int rem = n % T;

    double tstart = now_sec();

    for (int i = 0; i < T; ++i) {
        long extra = (i < rem) ? 1 : 0;
        long start = (long)i * base + (i < rem ? i : rem);
        long end = start + base + extra;
        args[i].start = start;
        args[i].end = end;
        args[i].fH = fH;
        int rc = pthread_create(&threads[i], NULL, thread_calc, &args[i]);
        if (rc != 0) {
            fprintf(stderr, "pthread_create failed (%d)\n", rc);
            return 1;
        }
    }

    double total_sum = 0.0;
    for (int i = 0; i < T; ++i) {
        void *rval;
        int rc = pthread_join(threads[i], &rval);
        if (rc != 0) {
            fprintf(stderr, "pthread_join failed (%d)\n", rc);
            return 1;
        }
        double *partial = (double *) rval;
        if (partial) {
            total_sum += *partial;
            free(partial);
        }
    }

    double tfinal = now_sec();

    double pi = fH * total_sum;
    const double PI_REF = 3.141592653589793238462643;

    printf("\npi is approximately = %.20f\n", pi);
    printf("Error               = %.20f\n", fabs(pi - PI_REF));
    printf("Threads: %d  Intervals: %ld\n", T, n);
    printf("Elapsed time (CalcPi region): %.6f seconds\n", tfinal - tstart);

    free(threads);
    free(args);
    return 0;
}
