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

// Estructura para pasar argumentos a cada hilo
typedef struct {
    long start;      // índice inicial (inclusive)
    long end;        // índice final (exclusivo)
    double fH;       // paso = 1.0 / n
} thread_arg_t;

// Función para obtener tiempo en segundos con alta precisión
static double now_sec(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

// Función que ejecuta cada hilo
void *thread_calc(void *arg)
{
    thread_arg_t *a = (thread_arg_t *)arg;
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

    // Uso del programa
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <num_threads> [n_intervals]\n", argv[0]);
        fprintf(stderr, "Ejemplo: %s 4 2000000000\n", argv[0]);
        return 1;
    }

    T = atoi(argv[1]);
    if (T <= 0) {
        fprintf(stderr, "num_threads debe ser > 0\n");
        return 1;
    }

    if (argc >= 3) {
        n = atol(argv[2]);
    }
    if (n <= 0) {
        fprintf(stderr, "n debe ser > 0\n");
        return 1;
    }

    const double fH = 1.0 / (double)n;
    const double PI25DT = 3.141592653589793238462643;

    // Reservar memoria para hilos y argumentos
    pthread_t *threads = malloc(sizeof(pthread_t) * T);
    thread_arg_t *args = malloc(sizeof(thread_arg_t) * T);
    if (!threads || !args) {
        perror("malloc");
        return 1;
    }

    // Distribución equitativa del trabajo (incluyendo resto)
    long base = n / T;
    int rem = n % T;

    // INICIO DE LA MEDICIÓN DEL TIEMPO (solo el cálculo paralelo)
    double tstart = now_sec();

    // Crear hilos
    for (int i = 0; i < T; ++i) {
        long extra = (i < rem) ? 1 : 0;
        long start = (long)i * base + (i < rem ? i : rem);
        long end = start + base + extra;

        args[i].start = start;
        args[i].end   = end;
        args[i].fH    = fH;

        int rc = pthread_create(&threads[i], NULL, thread_calc, &args[i]);
        if (rc != 0) {
            fprintf(stderr, "pthread_create falló (%d)\n", rc);
            return 1;
        }
    }

    // Recolectar resultados
    double total_sum = 0.0;
    for (int i = 0; i < T; ++i) {
        void *rval;
        int rc = pthread_join(threads[i], &rval);
        if (rc != 0) {
            fprintf(stderr, "pthread_join falló (%d)\n", rc);
            return 1;
        }
        double *partial = (double *)rval;
        if (partial) {
            total_sum += *partial;
            free(partial);
        }
    }

    // FIN DE LA MEDICIÓN
    double tend = now_sec();

    double pi = fH * total_sum;

    // RESULTADOS FINALES
    printf("\npi is approximately = %.20f\n", pi);
    printf("Error = %.20lf\n", fabs(pi - PI25DT));
    printf("Elapsed time (CalcPi): %.6f seconds\n", tend - tstart);

    // Liberar memoria
    free(threads);
    free(args);

    return 0;
}
#define _POSIX_C_SOURCE 199309L




































































































