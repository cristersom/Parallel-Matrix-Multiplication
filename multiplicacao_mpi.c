#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <sys/time.h> 

// Preenche uma matriz com valores aleatórios (0 ou 1).
// A matriz aqui é um vetor 1D para facilitar o envio de dados com MPI.
void gerar_matriz(int* matriz, int tamanho) {
    for (int i = 0; i < tamanho * tamanho; i++) {
        matriz[i] = rand() % 2;
    }
}

int main(int argc, char* argv[]) {
    int rank, world_size;
    int *matriz1 = NULL, *matriz2, *resultado = NULL; // Matrizes completas (só o mestre usa todas)
    int *chunk_matriz1, *chunk_resultado; // Pedaços de matriz para cada processo
    int tamanho;
    struct timeval start, end;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Pega o ID (rank) do processo atual
    MPI_Comm_size(MPI_COMM_WORLD, &world_size); // Pega o total de processos

    if (argc != 2) {
        if (rank == 0) { // Apenas o mestre imprime a mensagem de erro de uso
            fprintf(stderr, "Uso: mpirun -np <num_procs> %s <tamanho_da_matriz>\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    tamanho = atoi(argv[1]);
    // Divide o número de linhas da matriz pelo total de processos.
    int chunk_size = tamanho / world_size;

    // Alocação de memória para os blocos de dados de cada processo.
    chunk_matriz1 = (int*)malloc(chunk_size * tamanho * sizeof(int));
    chunk_resultado = (int*)malloc(chunk_size * tamanho * sizeof(int));
    matriz2 = (int*)malloc(tamanho * tamanho * sizeof(int)); // Todos precisam da matriz2 inteira

    // O processo mestre (rank 0) é responsável por criar os dados e distribuí-los.
    if (rank == 0) {
        printf("Iniciando multiplicação de matrizes PARALELA (MPI) %dx%d com %d processos.\n", tamanho, tamanho, world_size);
        
        matriz1 = (int*)malloc(tamanho * tamanho * sizeof(int));
        resultado = (int*)malloc(tamanho * tamanho * sizeof(int));

        printf("Etapa: Gerando matrizes...\n");
        srand(1); // Semente fixa para garantir a consistência dos dados entre testes
        gerar_matriz(matriz1, tamanho);
        gerar_matriz(matriz2, tamanho);
        printf("Etapa: Matrizes geradas.\n");

        printf("Etapa: Iniciando cálculo (distribuindo dados e cronometrando)...\n");
        gettimeofday(&start, NULL);
    }
    
    /* --- FASE DE DISTRIBUIÇÃO (COMMUNICATION) --- */
    
    // 1. Broadcast: O mestre envia a matriz2 inteira para TODOS os outros processos.
    MPI_Bcast(matriz2, tamanho * tamanho, MPI_INT, 0, MPI_COMM_WORLD);
    
    // 2. Scatter: O mestre distribui fatias da matriz1. Cada processo recebe um 'chunk' diferente.
    MPI_Scatter(matriz1, chunk_size * tamanho, MPI_INT, 
                chunk_matriz1, chunk_size * tamanho, MPI_INT, 0, MPI_COMM_WORLD);

    /* --- FASE DE CÁLCULO (COMPUTATION) --- */
    // Esta parte é executada em paralelo por todos os processos, cada um com seus dados.
    for (int i = 0; i < chunk_size; i++) {
        for (int j = 0; j < tamanho; j++) {
            int soma = 0;
            for (int k = 0; k < tamanho; k++) {
                // A[i][k] * B[k][j] - usando aritmética de ponteiros para simular o 2D
                soma += chunk_matriz1[i * tamanho + k] * matriz2[k * tamanho + j];
            }
            chunk_resultado[i * tamanho + j] = soma;
        }
    }

    /* --- FASE DE COLETA (COMMUNICATION) --- */
    // Gather: O mestre recolhe os 'chunks' de resultado de todos os processos e os junta.
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
        
        // Libera a memória que foi alocada apenas pelo mestre
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