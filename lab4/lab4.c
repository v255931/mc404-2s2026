#define STDIN_FD  0
#define STDOUT_FD 1

int read(int __fd, const void *__buf, int __n) {
  int ret_val;
  __asm__ __volatile__(
    "mv a0, %1           # file descriptor\n"
    "mv a1, %2           # buffer \n"
    "mv a2, %3           # size \n"
    "li a7, 63           # syscall write code (63) \n"
    "ecall               # invoke syscall \n"
    "mv %0, a0           # move return value to ret_val\n"
    : "=r"(ret_val)  // Output list
    : "r"(__fd), "r"(__buf), "r"(__n)    // Input list
    : "a0", "a1", "a2", "a7"
  );
  return ret_val;
}

void write(int __fd, const void *__buf, int __n) {
  __asm__ __volatile__(
    "mv a0, %0           # file descriptor\n"
    "mv a1, %1           # buffer \n"
    "mv a2, %2           # size \n"
    "li a7, 64           # syscall write (64) \n"
    "ecall"
    :   // Output list
    :"r"(__fd), "r"(__buf), "r"(__n)    // Input list
    : "a0", "a1", "a2", "a7"
  );
}

void exit(int code) {
  __asm__ __volatile__(
    "mv a0, %0           # return code\n"
    "li a7, 93           # syscall exit (64) \n"
    "ecall"
    :   // Output list
    :"r"(code)    // Input list
    : "a0", "a7"
  );
}

unsigned int multiply(unsigned int a, unsigned int b) {
  unsigned int result = 0;
  while (b > 0) {
    if (b & 1) {           
      result += a;         
    }
    a <<= 1;               
    b >>= 1;               
  }
  return result;
}

unsigned int divide(unsigned int dividend, unsigned int divisor) {
  if (divisor == 0) return 0;
  
  unsigned int quotient = 0;
  unsigned int rem = 0;

  for (int i = 31; i >= 0; i--) {
    unsigned int bit = (dividend >> i) & 1; 
    rem = (rem << 1) | bit;                 
    
    if (rem >= divisor) {
      rem -= divisor;                       
      quotient = (quotient << 1) | 1;       
    } else {
      quotient = (quotient << 1) | 0;     
    }
  }
  return quotient;
}

unsigned int remainder(unsigned int dividend, unsigned int divisor) {
  if (divisor == 0) return 0;
  
  unsigned int rem = 0;

  for (int i = 31; i >= 0; i--) {
    unsigned int bit = (dividend >> i) & 1;
    rem = (rem << 1) | bit;
    
    if (rem >= divisor) {
      rem -= divisor;
    }
  }
  return rem;
}

int main() {
  char buffer[10];
  int n = read(STDIN_FD, (void*) buffer, 10);
  
  if (n < 10) return 1;

  unsigned int a = 0;
  a = (a << 3) + (a << 1) + (buffer[0] - '0');
  a = (a << 3) + (a << 1) + (buffer[1] - '0');
  a = (a << 3) + (a << 1) + (buffer[2] - '0');

  char op = buffer[4];

  unsigned int b = 0;
  b = (b << 3) + (b << 1) + (buffer[6] - '0');
  b = (b << 3) + (b << 1) + (buffer[7] - '0');
  b = (b << 3) + (b << 1) + (buffer[8] - '0');

  unsigned int res = 0;
  if (op == '*') {
    res = multiply(a, b);
  } else if (op == '/') {
    res = divide(a, b);
  } else if (op == '%') {
    res = remainder(a, b);
  }

  char out_buf[8];
  unsigned int temp = res;
  
  for (int i = 6; i >= 0; i--) {
    unsigned int rem = remainder(temp, 10);
    out_buf[i] = rem + '0';       // Converte o dígito em caractere
    temp = divide(temp, 10);      // Divide temp por 10 para o próximo ciclo
  }
  out_buf[7] = '\n';

  write(STDOUT_FD, (void*) out_buf, 8);
  
  return 0;
}

void _start() {
  int ret_code = main();
  exit(ret_code);
}
