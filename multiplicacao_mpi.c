#include <stdio.h>
#include <stdlib.h>
#include <mpi.h> // Biblioteca principal do MPI

// Usaremos gettimeofday para consistência com as outras versões
#include <sys/time.h> 

/**
 * @brief Aloca uma matriz linearizada (vetor 1D) e a preenche com valores.
 */
void gerar_matriz(int* matriz, int tamanho) {
    for (int i = 0; i < tamanho * tamanho; i++) {
        matriz[i] = rand() % 2;
    }
}

int main(int argc, char* argv[]) {
    int rank, world_size;
    int** matriz1_2d = NULL; // Matriz 2D original (apenas para o mestre)
    int *matriz1, *matriz2, *resultado; // Matrizes linearizadas (1D)
    int *chunk_matriz1, *chunk_resultado; // Pedaços para cada processo
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
    int chunk_size = tamanho / world_size; // Quantas linhas cada processo receberá

    // Alocar memória para os "pedaços" que cada processo usará
    chunk_matriz1 = (int*)malloc(chunk_size * tamanho * sizeof(int));
    chunk_resultado = (int*)malloc(chunk_size * tamanho * sizeof(int));
    // Todos os processos precisam da matriz2 inteira
    matriz2 = (int*)malloc(tamanho * tamanho * sizeof(int));

    // --- MESTRE (rank 0): Prepara e distribui os dados ---
    if (rank == 0) {
        printf("Iniciando multiplicação de matrizes PARALELA (MPI) %dx%d com %d processos.\n", tamanho, tamanho, world_size);
        
        // Alocar as matrizes completas
        matriz1 = (int*)malloc(tamanho * tamanho * sizeof(int));
        resultado = (int*)malloc(tamanho * tamanho * sizeof(int));

        printf("Etapa: Gerando matrizes...\n");
        srand(1); // Usar semente fixa para garantir que todos tenham os mesmos dados
        gerar_matriz(matriz1, tamanho);
        gerar_matriz(matriz2, tamanho);
        printf("Etapa: Matrizes geradas.\n");

        printf("Etapa: Iniciando cálculo (distribuindo dados e cronometrando)...\n");
        gettimeofday(&start, NULL);
    }
    
    // --- DISTRIBUIÇÃO DOS DADOS ---
    // 1. Broadcast: Mestre envia matriz2 inteira para TODOS os processos.
    MPI_Bcast(matriz2, tamanho * tamanho, MPI_INT, 0, MPI_COMM_WORLD);
    
    // 2. Scatter: Mestre distribui "pedaços" da matriz1 para cada processo.
    MPI_Scatter(matriz1, chunk_size * tamanho, MPI_INT, 
                chunk_matriz1, chunk_size * tamanho, MPI_INT, 0, MPI_COMM_WORLD);

    // --- CÁLCULO PARALELO (Todos os processos executam esta parte) ---
    for (int i = 0; i < chunk_size; i++) {
        for (int j = 0; j < tamanho; j++) {
            int soma = 0;
            for (int k = 0; k < tamanho; k++) {
                // A[i][k] * B[k][j]
                soma += chunk_matriz1[i * tamanho + k] * matriz2[k * tamanho + j];
            }
            chunk_resultado[i * tamanho + j] = soma;
        }
    }

    // --- COLETA DOS DADOS ---
    // Gather: Todos os processos enviam seus 'chunk_resultado' para o mestre.
    // O mestre os junta na matriz 'resultado'.
    MPI_Gather(chunk_resultado, chunk_size * tamanho, MPI_INT, 
               resultado, chunk_size * tamanho, MPI_INT, 0, MPI_COMM_WORLD);

    // --- MESTRE (rank 0): Finaliza e mostra o resultado ---
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
        
        // Libera a memória alocada apenas pelo mestre
        free(matriz1);
        free(resultado);
    }

    // Libera a memória alocada por todos os processos
    free(matriz2);
    free(chunk_matriz1);
    free(chunk_resultado);
    
    MPI_Finalize();
    return 0;
}