#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <sys/time.h> 

void gerar_matriz(int* matriz, int tamanho) {
    for (int i = 0; i < tamanho * tamanho; i++) {
        matriz[i] = rand() % 2;
    }
}

void transpor_matriz_local(int* origem, int* destino, int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        for (int j = 0; j < tamanho; j++) {
            destino[j * tamanho + i] = origem[i * tamanho + j];
        }
    }
}

int main(int argc, char* argv[]) {
    int rank, world_size;
    int *matriz1 = NULL, *matriz2 = NULL, *matriz2_t = NULL, *resultado = NULL; 
    int *chunk_matriz1, *chunk_resultado; 
    int tamanho;
    struct timeval start, end;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
    MPI_Comm_size(MPI_COMM_WORLD, &world_size); 

    if (argc != 2) {
        if (rank == 0) fprintf(stderr, "Uso: %s <tamanho>\n", argv[0]);
        MPI_Finalize(); return 1;
    }

    tamanho = atoi(argv[1]);
    int chunk_size = tamanho / world_size;

    // Alocações locais
    chunk_matriz1 = (int*)malloc(chunk_size * tamanho * sizeof(int));
    chunk_resultado = (int*)malloc(chunk_size * tamanho * sizeof(int));
    // Matriz B Transposta (Necessária completa em todos os nós)
    matriz2_t = (int*)malloc(tamanho * tamanho * sizeof(int));

    if (rank == 0) {
        matriz1 = (int*)malloc(tamanho * tamanho * sizeof(int));
        matriz2 = (int*)malloc(tamanho * tamanho * sizeof(int));
        resultado = (int*)malloc(tamanho * tamanho * sizeof(int));

        srand(1);
        gerar_matriz(matriz1, tamanho);
        gerar_matriz(matriz2, tamanho);
        
        // Transpõe B apenas no mestre antes de enviar
        transpor_matriz_local(matriz2, matriz2_t, tamanho);
        
        gettimeofday(&start, NULL);
    }

    // 1. Envia a matriz B JÁ TRANSPOSTA para todos
    MPI_Bcast(matriz2_t, tamanho * tamanho, MPI_INT, 0, MPI_COMM_WORLD);
    
    // 2. Distribui fatias da Matriz A
    MPI_Scatter(matriz1, chunk_size * tamanho, MPI_INT, 
                chunk_matriz1, chunk_size * tamanho, MPI_INT, 0, MPI_COMM_WORLD);

    // KERNEL MPI OTIMIZADO
    for (int i = 0; i < chunk_size; i++) {
        int offset_A = i * tamanho;
        for (int j = 0; j < tamanho; j++) {
            int offset_B = j * tamanho; // Acesso linear em B_t
            int soma = 0;
            // Loop vetorizável
            for (int k = 0; k < tamanho; k++) {
                soma += chunk_matriz1[offset_A + k] * matriz2_t[offset_B + k];
            }
            chunk_resultado[offset_A + j] = soma;
        }
    }

    // 3. Coleta resultados
    MPI_Gather(chunk_resultado, chunk_size * tamanho, MPI_INT, 
               resultado, chunk_size * tamanho, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        gettimeofday(&end, NULL);
        double tempo = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) * 1e-6;
        printf("MPI | Processos: %d | Matriz: %dx%d | Tempo: %.6f s\n", world_size, tamanho, tamanho, tempo);
        
        free(matriz1); free(matriz2); free(resultado);
    }

    free(matriz2_t); free(chunk_matriz1); free(chunk_resultado);
    MPI_Finalize();
    return 0;
}