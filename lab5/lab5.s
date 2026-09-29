.globl _start

_start:
    jal main
    li a0, 0
    li a7, 93           # Syscall exit (93)
    ecall

main:
    addi sp, sp, -16
    sw ra, 0(sp)

    jal read

    la a1, input_address

    # Lê o primeiro número (A)
    li t0, 0            # t0 será o valor de A
    li t2, 10
parse_A:
    lbu t3, 0(a1)       # Carrega o byte atual
    addi a1, a1, 1      # Avança ponteiro para o próximo byte
    li t4, 32           # ASCII para espaço (' ')
    beq t3, t4, skip_spaces # Se for espaço, terminou o número A
    li t4, 10           # ASCII para newline ('\n')
    beq t3, t4, parse_B_init
    beqz t3, parse_B_init
    addi t3, t3, -48    # Converte ASCII para valor inteiro
    mul t0, t0, t2      # Multiplica acumulador por 10
    add t0, t0, t3      # Adiciona o novo dígito
    j parse_A

skip_spaces:
    lbu t3, 0(a1)
    li t4, 32
    bne t3, t4, parse_B_init # Se já não for espaço, começa a ler o B
    addi a1, a1, 1      # Se ainda for espaço, avança
    j skip_spaces

parse_B_init:
    li t1, 0            # t1 será o valor de B
parse_B:
    lbu t3, 0(a1)
    addi a1, a1, 1
    li t4, 10           # ASCII para newline
    beq t3, t4, end_parse # Se encontrar newline, terminou
    li t4, 32           # ASCII para espaço
    beq t3, t4, end_parse
    beqz t3, end_parse  # Se encontrar fim da string, terminou
    addi t3, t3, -48
    mul t1, t1, t2
    add t1, t1, t3
    j parse_B

end_parse:

    # Salva cópias dos valores originais para o cálculo final do MMC
    mv t3, t0           # t3 = A_original
    mv t4, t1           # t4 = B_original

    # 3. Calcula o MDC(A, B) via Algoritmo de Euclides
mdc_loop:
    beqz t1, mdc_end    # Enquanto B != 0
    rem t2, t0, t1      # R = A % B
    mv t0, t1           # A = B
    mv t1, t2           # B = R
    j mdc_loop

mdc_end:
    # Neste ponto, t0 contém o MDC(A, B)

    # 4. Calcula o MMC: MMC = (A * B) / MDC
    mul t5, t3, t4      # t5 = A_original * B_original
    div t5, t5, t0      # t5 = MMC

    # 5. Converte o inteiro do MMC para uma string de 4 dígitos (DDDD\n)
    la a1, result
    
    li t2, 10           # Restaura o valor 10 para as divisões

    # Dígito 3 (Unidade)
    rem t6, t5, t2
    addi t6, t6, 48
    sb t6, 3(a1)
    div t5, t5, t2

    # Dígito 2 (Dezena)
    rem t6, t5, t2
    addi t6, t6, 48
    sb t6, 2(a1)
    div t5, t5, t2

    # Dígito 1 (Centena)
    rem t6, t5, t2
    addi t6, t6, 48
    sb t6, 1(a1)
    div t5, t5, t2

    # Dígito 0 (Milhar)
    rem t6, t5, t2
    addi t6, t6, 48
    sb t6, 0(a1)

    # Adiciona a quebra de linha '\n' no final (byte 4)
    li t6, 10
    sb t6, 4(a1)

    # 6. Escreve a string de resultado (5 bytes) na saída padrão
    jal write

    # Restaura ra e finaliza main
    lw ra, 0(sp)
    addi sp, sp, 16
    ret

read:
    li a0, 0            # file descriptor = 0 (stdin)
    la a1, input_address # buffer
    li a2, 6            # size (6 bytes)
    li a7, 63           # syscall read (63)
    ecall
    ret

write:
    li a0, 1            # file descriptor = 1 (stdout)
    la a1, result       # buffer
    li a2, 5            # size (5 bytes: 4 dígitos + '\n')
    li a7, 64           # syscall write (64)
    ecall
    ret

.bss
input_address: .skip 0x06  # Buffer de entrada de 6 bytes
result:        .skip 0x05  # Buffer de saída de 5 bytes
