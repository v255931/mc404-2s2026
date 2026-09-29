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
    li t2, 10           # Multiplicador

    # Extrai o Número A
    li t0, 0            # Acumulador A
find_A:
    lbu t3, 0(a1)
    beqz t3, end_parse  # Fim da string
    addi a1, a1, 1
    li t4, 48
    blt t3, t4, find_A  # Se menor que '0' (espaço, \n, \r), ignora
    li t4, 57
    bgt t3, t4, find_A  # Se maior que '9', ignora
    
    # Encontrou o primeiro dígito de A
    addi t3, t3, -48
    add t0, t0, t3
read_A:
    lbu t3, 0(a1)
    li t4, 48
    blt t3, t4, find_B  # Não é dígito, então o A acabou. Vai procurar o B
    li t4, 57
    bgt t3, t4, find_B
    
    addi a1, a1, 1
    addi t3, t3, -48
    mul t0, t0, t2
    add t0, t0, t3
    j read_A

    # Extrai o Número B
find_B:
    li t1, 0            # Acumulador B
find_B_loop:
    lbu t3, 0(a1)
    beqz t3, end_parse
    addi a1, a1, 1
    li t4, 48
    blt t3, t4, find_B_loop
    li t4, 57
    bgt t3, t4, find_B_loop

    # Encontrou o primeiro dígito de B
    addi t3, t3, -48
    add t1, t1, t3
read_B:
    lbu t3, 0(a1)
    li t4, 48
    blt t3, t4, end_parse # Fim do B (encontrou \n, \r, etc)
    li t4, 57
    bgt t3, t4, end_parse

    addi a1, a1, 1
    addi t3, t3, -48
    mul t1, t1, t2
    add t1, t1, t3
    j read_B

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
    li a2, 32           
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
input_address: .skip 0x20  
result:        .skip 0x05  # Buffer de saída de 5 bytes
