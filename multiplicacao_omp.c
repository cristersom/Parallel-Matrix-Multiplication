#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <omp.h> // Biblioteca do OpenMP - ADICIONADA

/**
 * @brief Aloca dinamicamente uma matriz quadrada de inteiros.
 * * @param tamanho A dimensão (linhas e colunas) da matriz.
 * @return int** Ponteiro para a matriz alocada. Retorna NULL em caso de falha.
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

/**
 * @brief Libera a memória de uma matriz alocada dinamicamente.
 * * @param matriz A matriz a ser liberada.
 * @param tamanho A dimensão da matriz.
 */
void liberar_matriz(int** matriz, int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        free(matriz[i]);
    }
    free(matriz);
}

/**
 * @brief Preenche duas matrizes com valores aleatórios (0 ou 1).
 * * @param matriz1 Primeira matriz.
 * @param matriz2 Segunda matriz.
 * @param tamanho Dimensão das matrizes.
 */
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

/**
 * @brief Realiza a multiplicação de duas matrizes (C = A * B) em paralelo com OpenMP.
 * * @param matriz1 Matriz A.
 * @param matriz2 Matriz B.
 * @param resultado Matriz C (resultado).
 * @param tamanho Dimensão das matrizes.
 */
void multiplicar_matrizes(int** matriz1, int** matriz2, int** resultado, int tamanho) {
    // Diretiva OpenMP para paralelizar o laço for mais externo.
    // O trabalho de calcular as linhas da matriz resultado será dividido
    // entre as threads disponíveis.
    #pragma omp parallel for
    for (int linha = 0; linha < tamanho; linha++) {
        for (int coluna = 0; coluna < tamanho; coluna++) {
            int soma = 0;
            for (int k = 0; k < tamanho; k++) {
                soma += matriz1[linha][k] * matriz2[k][coluna];
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

    printf("Iniciando multiplicação de matrizes PARALELA (OpenMP) %dx%d\n", TAMANHO, TAMANHO);

    printf("Etapa: Alocando memória para as matrizes...\n");
    int** matriz1 = alocar_matriz(TAMANHO);
    int** matriz2 = alocar_matriz(TAMANHO);
    int** resultado = alocar_matriz(TAMANHO);

    if (matriz1 == NULL || matriz2 == NULL || resultado == NULL) {
        return 1;
    }
    printf("Etapa: Memória alocada com sucesso!\n");

    gerar_matrizes(matriz1, matriz2, TAMANHO);

    struct timeval start, end;
    printf("Etapa: Iniciando cálculo da multiplicação (esta parte será cronometrada)...\n");
    gettimeofday(&start, NULL);

    multiplicar_matrizes(matriz1, matriz2, resultado, TAMANHO);

    gettimeofday(&end, NULL);
    printf("Etapa: Cálculo finalizado.\n");

    long seconds = end.tv_sec - start.tv_sec;
    long microseconds = end.tv_usec - start.tv_usec;
    double tempo_execucao = seconds + microseconds * 1e-6;

    printf("\n---------- RESULTADO FINAL ----------\n");
    printf("Dimensões da Matriz: %dx%d\n", TAMANHO, TAMANHO);
    // omp_get_max_threads() é uma função do OpenMP que retorna o número de threads usadas.
    printf("Número de threads utilizadas: %d\n", omp_get_max_threads());
    printf("Tempo de execução da multiplicação: %.6f segundos\n", tempo_execucao);
    printf("-------------------------------------\n");

    printf("Etapa: Liberando memória...\n");
    liberar_matriz(matriz1, TAMANHO);
    liberar_matriz(matriz2, TAMANHO);
    liberar_matriz(resultado, TAMANHO);
    printf("Etapa: Memória liberada. Fim do programa.\n");

    return 0;
}