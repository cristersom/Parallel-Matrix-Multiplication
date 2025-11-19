#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <omp.h> // Inclui a biblioteca do OpenMP

/*
 * Aloca o espaço necessário para uma matriz NxN na memória heap.
 * Essencial para matrizes grandes que não caberiam na memória de pilha (stack).
 * Retorna um ponteiro para a matriz alocada.
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
            // Em caso de falha, libera a memória que já foi alocada.
            for (int j = 0; j < i; j++) {
                free(matriz[j]);
            }
            free(matriz);
            return NULL;
        }
    }
    return matriz;
}

// Libera a memória de uma matriz previamente alocada com alocar_matriz.
void liberar_matriz(int** matriz, int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        free(matriz[i]);
    }
    free(matriz);
}

// Preenche duas matrizes com valores aleatórios simples (0 ou 1).
void gerar_matrizes(int** matriz1, int** matriz2, int tamanho) {
    printf("Etapa: Gerando matrizes com valores aleatórios...\n");
    // Usar o tempo atual como semente garante números diferentes a cada execução.
    srand(time(NULL));
    for (int linha = 0; linha < tamanho; linha++) {
        for (int coluna = 0; coluna < tamanho; coluna++) {
            matriz1[linha][coluna] = rand() % 2;
            matriz2[linha][coluna] = rand() % 2;
        }
    }
    printf("Etapa: Matrizes geradas com sucesso!\n");
}

// Versão paralela do algoritmo de multiplicação usando OpenMP.
void multiplicar_matrizes(int** matriz1, int** matriz2, int** resultado, int tamanho) {
    // A diretiva 'pragma' a seguir é a chave da paralelização com OpenMP.
    // Ela instrui o compilador a dividir as iterações do primeiro laço (linhas)
    // entre as threads disponíveis, acelerando o processo.
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
    // omp_get_max_threads() é uma função do OpenMP que informa quantas threads foram usadas.
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