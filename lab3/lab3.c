#define STDIN_FD  0
#define STDOUT_FD 1

char input[33];

int read(int __fd, const void *__buf, int __n){
    int ret_val;
  __asm__ __volatile__(
    "mv a0, %1           # file descriptor\n"
    "mv a1, %2           # buffer \n"
    "mv a2, %3           # size \n"
    "li a7, 63           # syscall write code (63) \n"
    "ecall               # invoke syscall \n"
    "mv %0, a0           # move return value to ret_val\n"
    : "=r"(ret_val)  
    : "r"(__fd), "r"(__buf), "r"(__n)    
    : "a0", "a1", "a2", "a7"
  );
  return ret_val;
}

void write(int __fd, const void *__buf, int __n)
{
  __asm__ __volatile__(
    "mv a0, %0           # file descriptor\n"
    "mv a1, %1           # buffer \n"
    "mv a2, %2           # size \n"
    "li a7, 64           # syscall write (64) \n"
    "ecall"
    :   
    :"r"(__fd), "r"(__buf), "r"(__n)    
    : "a0", "a1", "a2", "a7"
  );
}

void exit(int code)
{
  __asm__ __volatile__(
    "mv a0, %0           # return code\n"
    "li a7, 93           # syscall exit (64) \n"
    "ecall"
    :   
    :"r"(code)    
    : "a0", "a7"
  );
}

// 1. Representação Decimal
void print_decimal(int num) {
    char buf[16];
    int i = 0;
    unsigned int n;
    
    if (num == 0) {
        buf[i++] = '0';
    } else {
        if (num < 0) {
            write(STDOUT_FD, "-", 1);
            n = -num;
        } else {
            n = num;
        }
        
        char temp[16];
        int j = 0;
        while (n > 0) {
            temp[j++] = (n % 10) + '0';
            n /= 10;
        }
        while (j > 0) {
            buf[i++] = temp[--j];
        }
    }
    buf[i++] = '\n';
    write(STDOUT_FD, buf, i);
}

// 2. Representação Binária (Big-endian)
void print_bin_be(int num) {
    char buf[36];
    buf[0] = '0'; buf[1] = 'b';
    for (int i = 0; i < 32; i++) {
        buf[2 + i] = ((num >> (31 - i)) & 1) + '0';
    }
    buf[34] = '\n';
    write(STDOUT_FD, buf, 35);
}

// 3. Representação Binária (Little-endian)
void print_bin_le(int num) {
    char buf[36];
    buf[0] = '0'; buf[1] = 'b';
    int idx = 2;
    // Imprime byte a byte (do menos significativo para o mais significativo)
    for (int byte = 0; byte < 4; byte++) {
        for (int bit = 7; bit >= 0; bit--) {
            buf[idx++] = ((num >> (byte * 8 + bit)) & 1) + '0';
        }
    }
    buf[idx++] = '\n';
    write(STDOUT_FD, buf, idx);
}

// 4. Representação Hexadecimal (Big-endian)
void print_hex_be(int num) {
    char buf[12];
    buf[0] = '0'; buf[1] = 'x';
    char hex[] = "0123456789abcdef";
    for(int i = 0; i < 8; i++) {
        buf[2 + i] = hex[(num >> (28 - i * 4)) & 0xF];
    }
    buf[10] = '\n';
    write(STDOUT_FD, buf, 11);
}

// 5. Representação Hexadecimal (Little-endian)
void print_hex_le(int num) {
    char buf[12];
    buf[0] = '0'; buf[1] = 'x';
    char hex[] = "0123456789abcdef";
    int idx = 2;
    for (int byte = 0; byte < 4; byte++) {
        buf[idx++] = hex[(num >> (byte * 8 + 4)) & 0xF];
        buf[idx++] = hex[(num >> (byte * 8)) & 0xF];
    }
    buf[idx++] = '\n';
    write(STDOUT_FD, buf, idx);
}

int main()
{
    // Leitura garantindo os 32 bits + \n
    read(STDIN_FD, (void*) input, 33);
    
    // Converte a string binária de 32 caracteres para um inteiro
    unsigned int ieee_val = 0;
    for (int i = 0; i < 32; i++) {
        ieee_val = (ieee_val << 1) | (input[i] - '0');
    }

    // Extrai componentes do IEEE 754
    unsigned int sign = (ieee_val >> 31) & 1;
    int exp = ((ieee_val >> 23) & 0xFF) - 127;
    unsigned int frac = ieee_val & 0x7FFFFF;
    
    int num = 0;

    // Calcula o valor inteiro truncando a parte fracionária
    if (exp >= 0) {
        unsigned int mantissa = frac | 0x800000; // Adiciona o 1 implícito
        if (exp > 23) {
            num = mantissa << (exp - 23);
        } else {
            num = mantissa >> (23 - exp);
        }
        if (sign) {
            num = -num; // Aplica o sinal
        }
    }

    print_decimal(num);
    print_bin_be(num);
    print_bin_le(num);
    print_hex_be(num);
    print_hex_le(num);

    return 0;
}

void _start()
{
  int ret_code = main();
  exit(ret_code);
}
