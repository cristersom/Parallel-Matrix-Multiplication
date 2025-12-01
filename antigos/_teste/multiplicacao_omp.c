#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <omp.h> // Inclui a biblioteca do OpenMP

/*
 * Funções auxiliares (alocar_matriz, liberar_matriz, gerar_matrizes) omitidas por brevidade,
 * mas são as mesmas da versão sequencial.
 */

int** alocar_matriz(int tamanho) {
    int** matriz = (int**)malloc(tamanho * sizeof(int*));
    if (matriz == NULL) {
        fprintf(stderr, "Erro de alocação de memória para as linhas.\n");
        return NULL;
    }
    for (int i = 0; i < tamanho; i++) {
        matriz[i] = (int*)malloc(tamanho * sizeof(int));
        if (matriz[i] == NULL) {
            fprintf(stderr, "Erro de alocação de memória para a linha %d.\n", i);
            for (int j = 0; j < i; j++) {
                free(matriz[j]);
            }
            free(matriz);
            return NULL;
        }
    }
    return matriz;
}

void liberar_matriz(int** matriz, int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        free(matriz[i]);
    }
    free(matriz);
}

void gerar_matrizes(int** matriz1, int** matriz2, int tamanho) {
    printf("Etapa: Gerando matrizes com valores aleatórios...\n");
    srand(time(NULL));
    for (int linha = 0; linha < tamanho; linha++) {
        for (int coluna = 0; coluna < tamanho; coluna++) {
            matriz1[linha][coluna] = rand() % 2;
            matriz2[linha][coluna] = rand() % 2;
        }
    }
    printf("Etapa: Matrizes geradas com sucesso!\n");
}

int** transpor_matriz(int** matriz, int tamanho) {
    int** matriz_t = alocar_matriz(tamanho);
    if (matriz_t == NULL) return NULL;

    for (int i = 0; i < tamanho; i++) {
        for (int j = 0; j < tamanho; j++) {
            matriz_t[i][j] = matriz[j][i];
        }
    }
    return matriz_t;
}


// Versão paralela do algoritmo de multiplicação usando OpenMP e Matriz Transposta.
void multiplicar_matrizes(int** matriz1, int** matriz2_t, int** resultado, int tamanho) {
    // #pragma omp parallel for é a chave da paralelização.
    // schedule(static) é geralmente o melhor para loops balanceados como este.
    #pragma omp parallel for schedule(static)
    for (int linha = 0; linha < tamanho; linha++) {
        for (int coluna = 0; coluna < tamanho; coluna++) {
            int soma = 0;
            for (int k = 0; k < tamanho; k++) {
                // Acesso otimizado: A[linha][k] * B_t[coluna][k]
                soma += matriz1[linha][k] * matriz2_t[coluna][k];
            }
            resultado[linha][coluna] = soma;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <tamanho_da_matriz>\n", argv[0]);
        return 1;
    }

    int TAMANHO = atoi(argv[1]);
    if (TAMANHO <= 0) {
        fprintf(stderr, "O tamanho da matriz deve ser um número inteiro positivo.\n");
        return 1;
    }

    printf("Iniciando multiplicação de matrizes PARALELA (OpenMP) OTMIZADA %dx%d\n", TAMANHO, TAMANHO);

    printf("Etapa: Alocando memória para as matrizes...\n");
    int** matriz1 = alocar_matriz(TAMANHO);
    int** matriz2 = alocar_matriz(TAMANHO);
    int** resultado = alocar_matriz(TAMANHO);
    int** matriz2_t = NULL; // Matriz transposta

    if (matriz1 == NULL || matriz2 == NULL || resultado == NULL) {
        if (matriz1) liberar_matriz(matriz1, TAMANHO);
        if (matriz2) liberar_matriz(matriz2, TAMANHO);
        if (resultado) liberar_matriz(resultado, TAMANHO);
        return 1;
    }
    printf("Etapa: Memória alocada com sucesso!\n");

    gerar_matrizes(matriz1, matriz2, TAMANHO);

    // Otimização de cache (fora da cronometragem)
    printf("Etapa: Transpondo Matriz 2 para otimização de cache...\n");
    matriz2_t = transpor_matriz(matriz2, TAMANHO);
    if (matriz2_t == NULL) {
        liberar_matriz(matriz1, TAMANHO);
        liberar_matriz(matriz2, TAMANHO);
        liberar_matriz(resultado, TAMANHO);
        return 1;
    }
    printf("Etapa: Matriz 2 transposta com sucesso!\n");


    struct timeval start, end;
    printf("Etapa: Iniciando cálculo da multiplicação (esta parte será cronometrada)...\n");
    gettimeofday(&start, NULL);

    multiplicar_matrizes(matriz1, matriz2_t, resultado, TAMANHO);

    gettimeofday(&end, NULL);
    printf("Etapa: Cálculo finalizado.\n");

    long seconds = end.tv_sec - start.tv_sec;
    long microseconds = end.tv_usec - start.tv_usec;
    double tempo_execucao = seconds + microseconds * 1e-6;

    printf("\n---------- RESULTADO FINAL ----------\n");
    printf("Dimensões da Matriz: %dx%d\n", TAMANHO, TAMANHO);
    printf("Número de threads utilizadas: %d\n", omp_get_max_threads());
    printf("Tempo de execução da multiplicação: %.6f segundos\n", tempo_execucao);
    printf("-------------------------------------\n");

    printf("Etapa: Liberando memória...\n");
    liberar_matriz(matriz1, TAMANHO);
    liberar_matriz(matriz2, TAMANHO);
    liberar_matriz(matriz2_t, TAMANHO);
    liberar_matriz(resultado, TAMANHO);
    printf("Etapa: Memória liberada. Fim do programa.\n");

    return 0;
}