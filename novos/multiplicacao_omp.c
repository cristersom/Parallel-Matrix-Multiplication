#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <omp.h>

int* alocar_matriz_linear(int tamanho) {
    int* matriz = (int*)calloc(tamanho * tamanho, sizeof(int));
    if (matriz == NULL) { fprintf(stderr, "Erro de alocação.\n"); exit(1); }
    return matriz;
}

void gerar_matrizes(int* matriz1, int* matriz2, int tamanho) {
    srand(time(NULL));
    #pragma omp parallel for
    for (int i = 0; i < tamanho * tamanho; i++) {
        matriz1[i] = rand() % 2;
        matriz2[i] = rand() % 2;
    }
}

void transpor_matriz(int* origem, int* destino, int tamanho) {
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < tamanho; i++) {
        for (int j = 0; j < tamanho; j++) {
            destino[j * tamanho + i] = origem[i * tamanho + j];
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) return 1;
    int TAMANHO = atoi(argv[1]);

    int* matriz1 = alocar_matriz_linear(TAMANHO);
    int* matriz2 = alocar_matriz_linear(TAMANHO);
    int* matriz2_t = alocar_matriz_linear(TAMANHO);
    int* resultado = alocar_matriz_linear(TAMANHO);

    gerar_matrizes(matriz1, matriz2, TAMANHO);
    transpor_matriz(matriz2, matriz2_t, TAMANHO);

    struct timeval start, end;
    gettimeofday(&start, NULL);

    // KERNEL OPENMP OTIMIZADO
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < TAMANHO; i++) {
        int offset_A = i * TAMANHO;
        for (int j = 0; j < TAMANHO; j++) {
            int offset_B = j * TAMANHO;
            int soma = 0;
            // Loop interno sem dependências, ideal para SIMD (AVX)
            for (int k = 0; k < TAMANHO; k++) {
                soma += matriz1[offset_A + k] * matriz2_t[offset_B + k];
            }
            resultado[offset_A + j] = soma;
        }
    }

    gettimeofday(&end, NULL);
    double tempo = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) * 1e-6;

    printf("OPENMP | Threads: %d | Matriz: %dx%d | Tempo: %.6f s\n", omp_get_max_threads(), TAMANHO, TAMANHO, tempo);

    free(matriz1); free(matriz2); free(matriz2_t); free(resultado);
    return 0;
}