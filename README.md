# Paralelização de Multiplicação de Matrizes

Este projeto foi desenvolvido para a disciplina de Sistemas Operacionais / Computação Paralela e tem como objetivo explorar a redução do tempo de execução do algoritmo de multiplicação de matrizes de grandes dimensões.

Foram implementadas três versões do algoritmo:
1.  **Sequencial:** Uma implementação padrão em C, servindo como baseline de performance.
2.  **Paralela com OpenMP:** Utiliza o paralelismo de memória compartilhada para acelerar o cálculo em uma única máquina com múltiplos núcleos.
3.  **Paralela com MPI:** Utiliza o paralelismo de memória distribuída, projetado para executar a tarefa em um cluster de múltiplos computadores.

## Resultados de Desempenho

Os testes abaixo foram executados em uma Máquina Virtual (VirtualBox) com 2 núcleos de CPU alocados. A versão OpenMP foi configurada para usar 4 threads (com oversubscription). Os resultados da versão MPI referem-se ao teste de desempenho final a ser executado no cluster do laboratório.

| Abordagem | Tamanho da Matriz | Tempo 1 (s) | Tempo 2 (s) | Tempo 3 (s) | **Média (s)** | **Speedup** |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **Sequencial** | 2000x2000 | 84.66 | 82.06 | 87.65 | **84.79** | **1.0x** |
| **OpenMP** | 2000x2000 | 43.50 | 47.07 | 46.45 | **45.67** | **1.86x** |
| **MPI (8 nós)** | 2000x2000 | 5.52 | 6.39 | 5.72 | **(a executar)** | **(a executar)** |
| **Sequencial** | 3000x3000 | 312.92 | 322.81 | 326.09 | **320.61** | **1.0x** |
| **OpenMP** | 3000x3000 | 169.39 | 174.45 | 175.93 | **173.26** | **1.85x** |
| **MPI (8 nós)**| 3000x3000 | 24.59 | 22.84 | 23.50 | **(a executar)** | **(a executar)** |
| **Sequencial** | 4000x4000 | 725.90 | 724.95 | 739.82 | **730.22** | **1.0x** |
| **OpenMP** | 4000x4000 | 428.86 | 421.90 | 427.34 | **426.03** | **1.71x** |
| **MPI (8 nós)**| 4000x4000 | 48.91 | 53.51 | 51.18 | **(a executar)** | **(a executar)** |


*Speedup = Tempo Médio Sequencial / Tempo Médio Paralelo*

## Como Usar o Projeto

Estas instruções são para um ambiente Linux baseado em Debian/Ubuntu.

### Pré-requisitos e Instalação

Para compilar e executar todos os códigos, você precisará dos seguintes pacotes: `build-essential` (que inclui o compilador GCC), `openmpi-bin` e `libopenmpi-dev`.

Você pode instalar tudo com um único comando:
```bash
sudo apt update
sudo apt install build-essential openmpi-bin libopenmpi-dev
```

### Compilação

Após clonar o repositório, compile cada versão do programa com os seguintes comandos:

```bash
# Para a versão Sequencial
gcc multiplicacao_sequencial.c -o sequencial

# Para a versão OpenMP (note a flag -fopenmp)
gcc -fopenmp multiplicacao_omp.c -o omp

# Para a versão MPI
mpicc multiplicacao_mpi.c -o mpi
```

### Execução

#### Versão Sequencial
```bash
./sequencial <tamanho_da_matriz>
# Exemplo:
./sequencial 2000
```

#### Versão OpenMP
Antes de executar, defina o número de threads que o OpenMP deve usar.
```bash
export OMP_NUM_THREADS=<numero_de_threads>
./omp <tamanho_da_matriz>
# Exemplo com 8 threads:
export OMP_NUM_THREADS=8
./omp 3000
```

#### Versão MPI (Teste Local)
Você pode testar a lógica do MPI em sua própria máquina, simulando múltiplos processos.
```bash
mpirun -np <numero_de_processos> ./mpi <tamanho_da_matriz>
# Exemplo com 2 processos:
mpirun -np 2 ./mpi 2000
```

## Instruções para o Teste de Desempenho em Cluster (MPI)

Este é o teste final para coletar os dados de desempenho da versão MPI e preencher os campos "(a executar)" da tabela de resultados. Ele deve ser realizado em um ambiente com múltiplos computadores em rede (cluster do laboratório).

### Fase 1: Preparação do Ambiente (Checklist)

1.  **Conectividade:** Garanta que todas as máquinas do cluster estejam na mesma rede e possam se comunicar (verifique com o comando `ping <ip_da_outra_maquina>`).
2.  **MPI Instalado:** Certifique-se de que o OpenMPI (`openmpi-bin`, `libopenmpi-dev`) está instalado em **todas** as máquinas.
3.  **Código no Mesmo Caminho:** O executável `mpi` precisa estar no **mesmo caminho absoluto** em todas as máquinas (ex: `/home/aluno/projeto_mpi/mpi`). Use `scp` para copiar a pasta do projeto para as outras máquinas.
4.  **Acesso SSH sem Senha:** O `mpirun` usa SSH para iniciar processos remotamente. Configure o acesso sem senha do nó mestre para todos os outros nós.
    * No nó mestre, gere uma chave: `ssh-keygen -t rsa`
    * Para cada nó trabalhador, copie a chave: `ssh-copy-id usuario@<ip_do_worker>`

### Fase 2: Execução

1.  **Crie o `hostfile`:** No nó mestre, dentro da pasta do projeto, crie um arquivo `hosts.txt` que lista os IPs de todas as máquinas do cluster, um por linha.

2.  **Execute o Teste:** Use o comando `mpirun`, especificando o número de processos e o `hostfile`.
    ```bash
    # Sintaxe: mpirun -np <total_de_processos> --hostfile hosts.txt ./mpi <tamanho_da_matriz>
    
    # Exemplo para o teste de 8 PCs com matriz 4000x4000
    mpirun -np 8 --hostfile hosts.txt ./mpi 4000
    ```
3.  **Colete os Dados:** Repita a execução 3 vezes para cada dimensão de matriz (2000, 3000, 4000) e anote os tempos para preencher a tabela de resultados.











(base) aluno@aluno-VirtualBox-so:~/Documents/Parallel-Matrix-Multiplication/mmp$ ./sequencial 2000
Iniciando multiplicação de matrizes sequencial 2000x2000
Etapa: Alocando memória para as matrizes...
Etapa: Memória alocada com sucesso!
Etapa: Gerando matrizes com valores aleatórios...
Etapa: Matrizes geradas com sucesso!
Etapa: Iniciando cálculo da multiplicação (esta parte será cronometrada)...
Etapa: Cálculo finalizado.

---------- RESULTADO FINAL ----------
Dimensões da Matriz: 2000x2000
Tempo de execução da multiplicação: 84.663450 segundos
-------------------------------------
Etapa: Liberando memória...
Etapa: Memória liberada. Fim do programa.
(base) aluno@aluno-VirtualBox-so:~/Documents/Parallel-Matrix-Multiplication/mmp$ ./sequencial 2000
Iniciando multiplicação de matrizes sequencial 2000x2000
Etapa: Alocando memória para as matrizes...
Etapa: Memória alocada com sucesso!
Etapa: Gerando matrizes com valores aleatórios...
Etapa: Matrizes geradas com sucesso!
Etapa: Iniciando cálculo da multiplicação (esta parte será cronometrada)...
Etapa: Cálculo finalizado.

---------- RESULTADO FINAL ----------
Dimensões da Matriz: 2000x2000
Tempo de execução da multiplicação: 82.063467 segundos
-------------------------------------
Etapa: Liberando memória...
Etapa: Memória liberada. Fim do programa.
(base) aluno@aluno-VirtualBox-so:~/Documents/Parallel-Matrix-Multiplication/mmp$ ./sequencial 2000
Iniciando multiplicação de matrizes sequencial 2000x2000
Etapa: Alocando memória para as matrizes...
Etapa: Memória alocada com sucesso!
Etapa: Gerando matrizes com valores aleatórios...
Etapa: Matrizes geradas com sucesso!
Etapa: Iniciando cálculo da multiplicação (esta parte será cronometrada)...
Etapa: Cálculo finalizado.

---------- RESULTADO FINAL ----------
Dimensões da Matriz: 2000x2000
Tempo de execução da multiplicação: 87.648286 segundos
-------------------------------------
Etapa: Liberando memória...
Etapa: Memória liberada. Fim do programa.
(base) aluno@aluno-VirtualBox-so:~/Documents/Parallel-Matrix-Multiplication/mmp$

(base) aluno@aluno-VirtualBox-so:~/Documents/Parallel-Matrix-Multiplication/mmp$ ./sequencial 3000
Iniciando multiplicação de matrizes sequencial 3000x3000
Etapa: Alocando memória para as matrizes...
Etapa: Memória alocada com sucesso!
Etapa: Gerando matrizes com valores aleatórios...
Etapa: Matrizes geradas com sucesso!
Etapa: Iniciando cálculo da multiplicação (esta parte será cronometrada)...
Etapa: Cálculo finalizado.

---------- RESULTADO FINAL ----------
Dimensões da Matriz: 3000x3000
Tempo de execução da multiplicação: 312.918163 segundos
-------------------------------------
Etapa: Liberando memória...
Etapa: Memória liberada. Fim do programa.
(base) aluno@aluno-VirtualBox-so:~/Documents/Parallel-Matrix-Multiplication/mmp$ ./sequencial 3000
Iniciando multiplicação de matrizes sequencial 3000x3000
Etapa: Alocando memória para as matrizes...
Etapa: Memória alocada com sucesso!
Etapa: Gerando matrizes com valores aleatórios...
Etapa: Matrizes geradas com sucesso!
Etapa: Iniciando cálculo da multiplicação (esta parte será cronometrada)...
Etapa: Cálculo finalizado.

---------- RESULTADO FINAL ----------
Dimensões da Matriz: 3000x3000
Tempo de execução da multiplicação: 322.806931 segundos
-------------------------------------
Etapa: Liberando memória...
Etapa: Memória liberada. Fim do programa.
(base) aluno@aluno-VirtualBox-so:~/Documents/Parallel-Matrix-Multiplication/mmp$ ./sequencial 3000
Iniciando multiplicação de matrizes sequencial 3000x3000
Etapa: Alocando memória para as matrizes...
Etapa: Memória alocada com sucesso!
Etapa: Gerando matrizes com valores aleatórios...
Etapa: Matrizes geradas com sucesso!
Etapa: Iniciando cálculo da multiplicação (esta parte será cronometrada)...
Etapa: Cálculo finalizado.

---------- RESULTADO FINAL ----------
Dimensões da Matriz: 3000x3000
Tempo de execução da multiplicação: 326.088063 segundos
-------------------------------------
Etapa: Liberando memória...
Etapa: Memória liberada. Fim do programa.
(base) aluno@aluno-VirtualBox-so:~/Documents/Parallel-Matrix-Multiplication/mmp$ 

(base) aluno@aluno-VirtualBox-so:~/Documents/Parallel-Matrix-Multiplication/mmp$ ./sequencial 4000
Iniciando multiplicação de matrizes sequencial 4000x4000
Etapa: Alocando memória para as matrizes...
Etapa: Memória alocada com sucesso!
Etapa: Gerando matrizes com valores aleatórios...
Etapa: Matrizes geradas com sucesso!
Etapa: Iniciando cálculo da multiplicação (esta parte será cronometrada)...
Etapa: Cálculo finalizado.

---------- RESULTADO FINAL ----------
Dimensões da Matriz: 4000x4000
Tempo de execução da multiplicação: 725.904766 segundos
-------------------------------------
Etapa: Liberando memória...
Etapa: Memória liberada. Fim do programa.
(base) aluno@aluno-VirtualBox-so:~/Documents/Parallel-Matrix-Multiplication/mmp$ ./sequencial 4000
Iniciando multiplicação de matrizes sequencial 4000x4000
Etapa: Alocando memória para as matrizes...
Etapa: Memória alocada com sucesso!
Etapa: Gerando matrizes com valores aleatórios...
Etapa: Matrizes geradas com sucesso!
Etapa: Iniciando cálculo da multiplicação (esta parte será cronometrada)...
Etapa: Cálculo finalizado.

---------- RESULTADO FINAL ----------
Dimensões da Matriz: 4000x4000
Tempo de execução da multiplicação: 724.945403 segundos
-------------------------------------
Etapa: Liberando memória...
Etapa: Memória liberada. Fim do programa.
(base) aluno@aluno-VirtualBox-so:~/Documents/Parallel-Matrix-Multiplication/mmp$ ./sequencial 4000
Iniciando multiplicação de matrizes sequencial 4000x4000
Etapa: Alocando memória para as matrizes...
Etapa: Memória alocada com sucesso!
Etapa: Gerando matrizes com valores aleatórios...
Etapa: Matrizes geradas com sucesso!
Etapa: Iniciando cálculo da multiplicação (esta parte será cronometrada)...
Etapa: Cálculo finalizado.

---------- RESULTADO FINAL ----------
Dimensões da Matriz: 4000x4000
Tempo de execução da multiplicação: 739.819729 segundos
-------------------------------------
Etapa: Liberando memória...
Etapa: Memória liberada. Fim do programa.
(base) aluno@aluno-VirtualBox-so:~/Documents/Parallel-Matrix-Multiplication/mmp$

(base) aluno@aluno-VirtualBox-so:~/Documents/Parallel-Matrix-Multiplication/mmp$ ./omp 2000
Iniciando multiplicação de matrizes PARALELA (OpenMP) 2000x2000
Etapa: Alocando memória para as matrizes...
Etapa: Memória alocada com sucesso!
Etapa: Gerando matrizes com valores aleatórios...
Etapa: Matrizes geradas com sucesso!
Etapa: Iniciando cálculo da multiplicação (esta parte será cronometrada)...
Etapa: Cálculo finalizado.

---------- RESULTADO FINAL ----------
Dimensões da Matriz: 2000x2000
Número de threads utilizadas: 4
Tempo de execução da multiplicação: 43.504357 segundos
-------------------------------------
Etapa: Liberando memória...
Etapa: Memória liberada. Fim do programa.
(base) aluno@aluno-VirtualBox-so:~/Documents/Parallel-Matrix-Multiplication/mmp$ ./omp 2000
Iniciando multiplicação de matrizes PARALELA (OpenMP) 2000x2000
Etapa: Alocando memória para as matrizes...
Etapa: Memória alocada com sucesso!
Etapa: Gerando matrizes com valores aleatórios...
Etapa: Matrizes geradas com sucesso!
Etapa: Iniciando cálculo da multiplicação (esta parte será cronometrada)...
Etapa: Cálculo finalizado.

---------- RESULTADO FINAL ----------
Dimensões da Matriz: 2000x2000
Número de threads utilizadas: 4
Tempo de execução da multiplicação: 47.071681 segundos
-------------------------------------
Etapa: Liberando memória...
Etapa: Memória liberada. Fim do programa.
(base) aluno@aluno-VirtualBox-so:~/Documents/Parallel-Matrix-Multiplication/mmp$ ./omp 2000
Iniciando multiplicação de matrizes PARALELA (OpenMP) 2000x2000
Etapa: Alocando memória para as matrizes...
Etapa: Memória alocada com sucesso!
Etapa: Gerando matrizes com valores aleatórios...
Etapa: Matrizes geradas com sucesso!
Etapa: Iniciando cálculo da multiplicação (esta parte será cronometrada)...
Etapa: Cálculo finalizado.

---------- RESULTADO FINAL ----------
Dimensões da Matriz: 2000x2000
Número de threads utilizadas: 4
Tempo de execução da multiplicação: 46.449374 segundos
-------------------------------------
Etapa: Liberando memória...
Etapa: Memória liberada. Fim do programa.
(base) aluno@aluno-VirtualBox-so:~/Documents/Parallel-Matrix-Multiplication/mmp$ 

(base) aluno@aluno-VirtualBox-so:~/Documents/Parallel-Matrix-Multiplication/mmp$ ./omp 3000
Iniciando multiplicação de matrizes PARALELA (OpenMP) 3000x3000
Etapa: Alocando memória para as matrizes...
Etapa: Memória alocada com sucesso!
Etapa: Gerando matrizes com valores aleatórios...
Etapa: Matrizes geradas com sucesso!
Etapa: Iniciando cálculo da multiplicação (esta parte será cronometrada)...
Etapa: Cálculo finalizado.

---------- RESULTADO FINAL ----------
Dimensões da Matriz: 3000x3000
Número de threads utilizadas: 4
Tempo de execução da multiplicação: 169.391762 segundos
-------------------------------------
Etapa: Liberando memória...
Etapa: Memória liberada. Fim do programa.
(base) aluno@aluno-VirtualBox-so:~/Documents/Parallel-Matrix-Multiplication/mmp$ ./omp 3000
Iniciando multiplicação de matrizes PARALELA (OpenMP) 3000x3000
Etapa: Alocando memória para as matrizes...
Etapa: Memória alocada com sucesso!
Etapa: Gerando matrizes com valores aleatórios...
Etapa: Matrizes geradas com sucesso!
Etapa: Iniciando cálculo da multiplicação (esta parte será cronometrada)...
Etapa: Cálculo finalizado.

---------- RESULTADO FINAL ----------
Dimensões da Matriz: 3000x3000
Número de threads utilizadas: 4
Tempo de execução da multiplicação: 174.446146 segundos
-------------------------------------
Etapa: Liberando memória...
Etapa: Memória liberada. Fim do programa.
(base) aluno@aluno-VirtualBox-so:~/Documents/Parallel-Matrix-Multiplication/mmp$ ./omp 3000
Iniciando multiplicação de matrizes PARALELA (OpenMP) 3000x3000
Etapa: Alocando memória para as matrizes...
Etapa: Memória alocada com sucesso!
Etapa: Gerando matrizes com valores aleatórios...
Etapa: Matrizes geradas com sucesso!
Etapa: Iniciando cálculo da multiplicação (esta parte será cronometrada)...
Etapa: Cálculo finalizado.

---------- RESULTADO FINAL ----------
Dimensões da Matriz: 3000x3000
Número de threads utilizadas: 4
Tempo de execução da multiplicação: 175.927226 segundos
-------------------------------------
Etapa: Liberando memória...
Etapa: Memória liberada. Fim do programa.
(base) aluno@aluno-VirtualBox-so:~/Documents/Parallel-Matrix-Multiplication/mmp$ 

(base) aluno@aluno-VirtualBox-so:~/Documents/Parallel-Matrix-Multiplication/mmp$ ./omp 4000
Iniciando multiplicação de matrizes PARALELA (OpenMP) 4000x4000
Etapa: Alocando memória para as matrizes...
Etapa: Memória alocada com sucesso!
Etapa: Gerando matrizes com valores aleatórios...
Etapa: Matrizes geradas com sucesso!
Etapa: Iniciando cálculo da multiplicação (esta parte será cronometrada)...
Etapa: Cálculo finalizado.

---------- RESULTADO FINAL ----------
Dimensões da Matriz: 4000x4000
Número de threads utilizadas: 4
Tempo de execução da multiplicação: 428.860039 segundos
-------------------------------------
Etapa: Liberando memória...
Etapa: Memória liberada. Fim do programa.
(base) aluno@aluno-VirtualBox-so:~/Documents/Parallel-Matrix-Multiplication/mmp$ ./omp 4000
Iniciando multiplicação de matrizes PARALELA (OpenMP) 4000x4000
Etapa: Alocando memória para as matrizes...
Etapa: Memória alocada com sucesso!
Etapa: Gerando matrizes com valores aleatórios...
Etapa: Matrizes geradas com sucesso!
Etapa: Iniciando cálculo da multiplicação (esta parte será cronometrada)...
Etapa: Cálculo finalizado.

---------- RESULTADO FINAL ----------
Dimensões da Matriz: 4000x4000
Número de threads utilizadas: 4
Tempo de execução da multiplicação: 421.904753 segundos
-------------------------------------
Etapa: Liberando memória...
Etapa: Memória liberada. Fim do programa.
(base) aluno@aluno-VirtualBox-so:~/Documents/Parallel-Matrix-Multiplication/mmp$ ./omp 4000
Iniciando multiplicação de matrizes PARALELA (OpenMP) 4000x4000
Etapa: Alocando memória para as matrizes...
Etapa: Memória alocada com sucesso!
Etapa: Gerando matrizes com valores aleatórios...
Etapa: Matrizes geradas com sucesso!
Etapa: Iniciando cálculo da multiplicação (esta parte será cronometrada)...
Etapa: Cálculo finalizado.

---------- RESULTADO FINAL ----------
Dimensões da Matriz: 4000x4000
Número de threads utilizadas: 4
Tempo de execução da multiplicação: 427.335482 segundos
-------------------------------------
Etapa: Liberando memória...
Etapa: Memória liberada. Fim do programa.
