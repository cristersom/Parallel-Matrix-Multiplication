#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <sys/time.h> 

// Preenche uma matriz com valores aleatórios (0 ou 1) no formato 1D.
void gerar_matriz(int* matriz, int tamanho) {
    for (int i = 0; i < tamanho * tamanho; i++) {
        matriz[i] = rand() % 2;
    }
}

/*
 * Transpõe uma matriz armazenada em formato 1D (por linha)
 * para otimizar o acesso à memória na multiplicação (cache).
 */
void transpor_matriz_1d(int* matriz, int tamanho) {
    // Aloca um buffer para a matriz transposta
    int* matriz_t = (int*)malloc(tamanho * tamanho * sizeof(int));
    if (matriz_t == NULL) {
        fprintf(stderr, "Erro de alocação de memória para a transposição.\n");
        return;
    }

    for (int i = 0; i < tamanho; i++) { // Linhas originais
        for (int j = 0; j < tamanho; j++) { // Colunas originais
            // A matriz original na posição (i, j) é matriz[i * tamanho + j]
            // A transposta na posição (j, i) é matriz_t[j * tamanho + i]
            matriz_t[j * tamanho + i] = matriz[i * tamanho + j];
        }
    }
    
    // Copia os dados transpostos de volta para o array original (matriz)
    for (int i = 0; i < tamanho * tamanho; i++) {
        matriz[i] = matriz_t[i];
    }

    free(matriz_t);
}

int main(int argc, char* argv[]) {
    int rank, world_size;
    int *matriz1 = NULL, *matriz2, *resultado = NULL; 
    int *chunk_matriz1, *chunk_resultado; 
    int tamanho;
    struct timeval start, end;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
    MPI_Comm_size(MPI_COMM_WORLD, &world_size); 

    if (argc != 2) {
        if (rank == 0) { 
            fprintf(stderr, "Uso: mpirun -np <num_procs> %s <tamanho_da_matriz>\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    tamanho = atoi(argv[1]);
    if (tamanho % world_size != 0) {
        if (rank == 0) {
            fprintf(stderr, "Erro: O tamanho da matriz (%d) deve ser divisível pelo número de processos (%d).\n", tamanho, world_size);
        }
        MPI_Finalize();
        return 1;
    }
    
    int chunk_size = tamanho / world_size;

    // Alocação de memória para os blocos de dados de cada processo.
    chunk_matriz1 = (int*)malloc(chunk_size * tamanho * sizeof(int));
    chunk_resultado = (int*)malloc(chunk_size * tamanho * sizeof(int));
    matriz2 = (int*)malloc(tamanho * tamanho * sizeof(int)); // Todos precisam da matriz2 (transposta) inteira

    // O processo mestre (rank 0) é responsável por criar os dados e distribuí-los.
    if (rank == 0) {
        printf("Iniciando multiplicação de matrizes PARALELA (MPI) OTMIZADA %dx%d com %d processos.\n", tamanho, tamanho, world_size);
        
        matriz1 = (int*)malloc(tamanho * tamanho * sizeof(int));
        resultado = (int*)malloc(tamanho * tamanho * sizeof(int));

        printf("Etapa: Gerando matrizes...\n");
        srand(1); // Semente fixa
        gerar_matriz(matriz1, tamanho);
        gerar_matriz(matriz2, tamanho); // Gera a matriz B

        // Otimização de cache (fora da cronometragem)
        printf("Etapa: Transpondo Matriz 2...\n");
        transpor_matriz_1d(matriz2, tamanho); // Transpõe B -> B_t
        printf("Etapa: Matriz 2 transposta.\n");

        printf("Etapa: Iniciando cálculo (distribuindo dados e cronometrando)...\n");
        gettimeofday(&start, NULL);
    }
    
    /* --- FASE DE DISTRIBUIÇÃO (COMMUNICATION) --- */
    
    // 1. Broadcast: O mestre envia a matriz2_t inteira para TODOS (matriz2 agora contém a transposta).
    MPI_Bcast(matriz2, tamanho * tamanho, MPI_INT, 0, MPI_COMM_WORLD);
    
    // 2. Scatter: O mestre distribui fatias da matriz1.
    MPI_Scatter(matriz1, chunk_size * tamanho, MPI_INT, 
                chunk_matriz1, chunk_size * tamanho, MPI_INT, 0, MPI_COMM_WORLD);

    /* --- FASE DE CÁLCULO (COMPUTATION) --- */
    // A fórmula é C[i][j] = A_chunk[i][k] * B_t[j][k]
    for (int i = 0; i < chunk_size; i++) {
        for (int j = 0; j < tamanho; j++) {
            int soma = 0;
            for (int k = 0; k < tamanho; k++) {
                // Acesso otimizado: A[i][k] * B_t[j][k]
                // B_t[j * tamanho + k] é o acesso sequencial à linha 'j' da matriz transposta.
                soma += chunk_matriz1[i * tamanho + k] * matriz2[j * tamanho + k];
            }
            chunk_resultado[i * tamanho + j] = soma;
        }
    }

    /* --- FASE DE COLETA (COMMUNICATION) --- */
    // Gather: O mestre recolhe os 'chunks' de resultado.
    MPI_Gather(chunk_resultado, chunk_size * tamanho, MPI_INT, 
               resultado, chunk_size * tamanho, MPI_INT, 0, MPI_COMM_WORLD);

    // O mestre finaliza a contagem de tempo e exibe o resultado.
    if (rank == 0) {
        gettimeofday(&end, NULL);
        printf("Etapa: Cálculo finalizado.\n");

        long seconds = end.tv_sec - start.tv_sec;
        long microseconds = end.tv_usec - start.tv_usec;
        double tempo_execucao = seconds + microseconds * 1e-6;

        printf("\n---------- RESULTADO FINAL ----------\n");
        printf("Dimensões da Matriz: %dx%d\n", tamanho, tamanho);
        printf("Número de processos: %d\n", world_size);
        printf("Tempo de execução da multiplicação: %.6f segundos\n", tempo_execucao);
        printf("-------------------------------------\n");
        
        // Libera a memória alocada pelo mestre
        free(matriz1);
        free(resultado);
    }

    // Todos os processos liberam a memória que alocaram
    free(matriz2);
    free(chunk_matriz1);
    free(chunk_resultado);
    
    MPI_Finalize(); // Finaliza o ambiente MPI
    return 0;
}