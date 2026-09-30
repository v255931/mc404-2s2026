# ============================================================
# 1D Fluid Diffusion - RV32I
#
# Formula:
#   difference = left + right - 2 * center
#   change     = difference >> 2
#   new_value  = center + change
#
# The first and last elements remain unchanged.
# All test cases have a number of elements >= 3.

# ============================================================
# Constants Definition

.equ MAX_SHAPE, 128
.equ MAX_INPUT, MAX_SHAPE * 3

# ============================================================
# Data Segment

.data

main_buffer:   .space MAX_SHAPE * 4
aux_buffer:    .space MAX_SHAPE * 4
input_buffer:  .space MAX_INPUT
output_buffer: .space MAX_INPUT

# ============================================================
# Code Segment

.text

.globl _start

_start:

    # read from Linux stdin
    jal read_input_procedure
    # Return: array, n, iterations

    # execute a diffusion_1d
    # Uses: array, n, iterations
    jal diffusion_1d

    # write on Linux stdout
    # Uses: array, n
    jal write_input_procedure

    # syscall exit
    li a0, 0
    li a7, 93
    ecall

diffusion_1d:


read_input_procedure:
    li a0, 0            # file descriptor = 0 (stdin)
    la a1, input_buffer # buffer
    li a2, 32
    li a7, 63           # syscall read (63)
    ecall
    ret

write_input_procedure:
    li a0, 1            # file descriptor = 1 (stdout)
    la a1, output_buffer       # buffer
    li a2, 32               
    li a7, 64           # syscall write (64)
    ecall
    ret
