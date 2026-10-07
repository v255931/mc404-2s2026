.data
    # Buffers na memória para ler a entrada e gravar a saída
    input_buffer:   .skip 32
    output_buffer:  .skip 12

.text
.globl _start

_start:
  # TODO: Ler a entrada do usuário (coordenadas e tempos)
  j read

  # TODO: Implementar o cálculo das distâncias
  j calculate_distances

  # TODO: Converter os resultados de X e Y (inteiros) de volta
  j write

  li a0, 0
  li a7, 93                   # syscall de exit (93)
  ecall


# ============================================================
# Método auxiliar: Raiz Quadrada (Método Babilônico)
# ============================================================
# Entrada: a0 (Valor que você deseja calcular a raiz)
# Saída:   a0 (Resultado inteiro da raiz quadrada)
# ============================================================
sqrt:
    li t2, 1
    ble a0, t2, sqrt_end 

    li t2, 21                   # t2 = Contador de iterações (precisão)
    mv t0, a0                   # Copia o valor de entrada para t0
    srli t0, t0, 1              # t0 = Chute inicial (Entrada / 2)

loop_sqrt:
    div t1, a0, t0              # t1 = Entrada / Chute
    add t1, t1, t0              # t1 = Chute + (Entrada / Chute)
    srli t0, t1, 1              # t0 = Novo Chute (Divide tudo por 2)

    addi t2, t2, -1             # Diminui o contador de iterações
    bnez t2, loop_sqrt          # Se não chegou a zero, repete o loop

    mv a0, t0                   # Coloca o resultado final em a0

sqrt_end:
    ret
