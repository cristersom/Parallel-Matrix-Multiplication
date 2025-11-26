# Paralelização de Multiplicação de Matrizes

Este projeto foi desenvolvido para a disciplina de Sistemas Operacionais / Computação Paralela e tem como objetivo explorar a redução do tempo de execução do algoritmo de multiplicação de matrizes de grandes dimensões.

Foram implementadas três versões do algoritmo com otimizações de memória (vetor linear) e flags de compilação agressivas:
1.  **Sequencial:** Uma implementação otimizada em C, servindo como baseline de performance.
2.  **Paralela com OpenMP:** Utiliza o paralelismo de memória compartilhada para acelerar o cálculo em uma única máquina com múltiplos núcleos.
3.  **Paralela com MPI:** Utiliza o paralelismo de memória distribuída, projetado para executar a tarefa em um cluster de múltiplos computadores.

## Resultados de Desempenho

Os testes abaixo foram executados utilizando flags de otimização (`-O3`, `-march=native`). A versão OpenMP foi configurada para usar **4 threads**.

| Abordagem | Tamanho da Matriz | Tempo 1 (s) | Tempo 2 (s) | Tempo 3 (s) | **Média (s)** | **Speedup** |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **Sequencial** | 2000x2000 | 2.07 | 1.83 | 1.83 | **1.91** | **1.0x** |
| **OpenMP** | 2000x2000 | 0.74 | 0.63 | 0.61 | **0.66** | **2.89x** |
| **MPI (8 nós)** | 2000x2000 | 2.10 | 2.03 | 2.02 | **2.05** | **0.93x** |
| **Sequencial** | 3000x3000 | 6.86 | 6.87 | 6.87 | **6.87** | **1.0x** |
| **OpenMP** | 3000x3000 | 2.22 | 2.17 | 2.14 | **2.18** | **3.15x** |
| **MPI (8 nós)**| 3000x3000 | 4.82 | 4.80 | 4.79 | **4.80** | **1.43x** |
| **Sequencial** | 4000x4000 | 16.94 | 17.03 | 16.93 | **16.97** | **1.0x** |
| **OpenMP** | 4000x4000 | 5.17 | 5.33 | 5.17 | **5.22** | **3.25x** |
| **MPI (8 nós)**| 4000x4000 | 8.95 | 8.83 | 9.01 | **8.93** | **1.90x** |

*Speedup = Tempo Médio Sequencial / Tempo Médio Paralelo*
*(Nota: O MPI apresenta overhead de rede perceptível em matrizes menores, mas escala melhor conforme o tamanho do problema aumenta)*

## Como Usar o Projeto

Estas instruções são para um ambiente Linux baseado em Debian/Ubuntu.

### Pré-requisitos e Instalação

Para compilar e executar todos os códigos, você precisará dos seguintes pacotes: `build-essential` (que inclui o compilador GCC), `openmpi-bin` e `libopenmpi-dev`.

Você pode instalar tudo com um único comando:
```bash
sudo apt update
sudo apt install build-essential openmpi-bin libopenmpi-dev

# Para a versão Sequencial
gcc -O3 -march=native -ffast-math -funroll-loops multiplicacao_sequencial.c -o sequencial

# Para a versão OpenMP
gcc -fopenmp -O3 -march=native -ffast-math -funroll-loops multiplicacao_omp.c -o omp

# Para a versão MPI
mpicc -O3 -march=native -ffast-math -funroll-loops multiplicacao_mpi.c -o mpi

./sequencial <tamanho_da_matriz>
# Exemplo:
./sequencial 2000

export OMP_NUM_THREADS=4
./omp <tamanho_da_matriz>

# Exemplo:
./omp 2000

mpirun -np 4 ./mpi 2000

mpirun --hostfile hosts.txt -np 16 ./mpi 4000

# Exemplo de hosts.txt
192.168.0.10 slots=4
192.168.0.11 slots=4
...

mpirun --hostfile hosts.txt -np <total_de_processos> ./mpi <tamanho_matriz>
