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

    # Converte o primeiro número A (bytes 0 e 1)
    lbu t0, 0(a1)       # Caractere da dezena de A
    addi t0, t0, -48    # Converte ASCII '0'-'9' para valor 0-9
    li t2, 10
    mul t0, t0, t2      # Dezena * 10
    lbu t1, 1(a1)       # Caractere da unidade de A
    addi t1, t1, -48    # Converte ASCII para valor
    add t0, t0, t1      # t0 = valor de A

    # Converte o segundo número B (bytes 3 e 4)
    lbu t1, 3(a1)       # Caractere da dezena de B
    addi t1, t1, -48
    mul t1, t1, t2      # Dezena * 10
    lbu t3, 4(a1)       # Caractere da unidade de B
    addi t3, t3, -48
    add t1, t1, t3      # t1 = valor de B

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
