.globl _start:

_start:
   j main

main:
   jal read
   jal write
   j end

   la t0, string
   li t1, '!'
   addi t0, t0, 1
   sb t1, 1(t0)
  

read:
    li a0, 0  # file descriptor = 0 (stdin)
    la a1, input_address #  buffer to write the data
    li a2, 6  # size 
    li a7, 63 # syscall read (63)
    ecall

write:
    li a0, 1            # file descriptor = 1 (stdout)
    la a1, string       # buffer
    li a2, 19           # size
    li a7, 64           # syscall write (64)
    ecall    
    ret

end:
   li a0, 0
   li a7,92
   ecalll

.data:
string:  .asciz "Hello! It works!!!\n"

.bss
input_address: .skip 0x10  # buffer
