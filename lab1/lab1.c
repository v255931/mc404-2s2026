#define STDIN_FD  0
#define STDOUT_FD 1

/* Buffer para leitura de dados */
char input[8];
char resultado[6] = "00000";
 
char quebra_linha[] = "\n";

int main();

void exit(int code)
{
  __asm__ __volatile__(
    "mv a0, %0           # return code\n"
    "li a7, 93           # syscall exit (64) \n"
    "ecall"
    :             // Output list
    :"r"(code)    // Input list
    : "a0", "a7"
  );
  while (1);
}

void _start()
{
  int ret_code = main();
  exit(ret_code);
}

/* read
 * Parâmetros:
 *  __fd:  file descriptor do arquivo a ser lido.
 *  __buf: buffer para armazenar o dado lido.
 *  __n:   quantidade máxima de bytes a serem lidos.
 * Retorno:
 *  Número de bytes lidos.
 */
int read(int __fd, const void *__buf, int __n)
{
    int ret_val;
  __asm__ __volatile__(
    "mv a0, %1           # file descriptor\n"
    "mv a1, %2           # buffer \n"
    "mv a2, %3           # size \n"
    "li a7, 63           # syscall read code (63) \n"
    "ecall               # invoke syscall \n"
    "mv %0, a0           # move return value to ret_val\n"
    : "=r"(ret_val)                   // Output list
    : "r"(__fd), "r"(__buf), "r"(__n) // Input list
    : "a0", "a1", "a2", "a7"
  );
  return ret_val;
}

/* write
 * Parâmetros:
 *  __fd:  files descriptor para escrita dos dados.
 *  __buf: buffer com dados a serem escritos.
 *  __n:   quantidade de bytes a serem escritos.
 * Retorno:
 *  Número de bytes efetivamente escritos.
 */
void write(int __fd, const void *__buf, int __n)
{
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

void formatar_resultado(int valor) {
  if (valor < 0) valor = 0;

  for (int i=4; i >= 0; i--) {
    resultado[i] = (valor % 10) + '0';
    valor /= 10;
  }
  resultado[5] = '\0';
}

void somar(int a, int b) {
  formatar_resultado(a+b);
}

void subtrair(int a, int b) {
  formatar_resultado(a-b);
}

void multiplicar(int a, int b) {
  formatar_resultado(a*b);
}

void dividir(int a, int b) {
  formatar_resultado(a/b);
}

int main()
{
  int n = read(STDIN_FD, (void*) input, 8);

  int num1 = (input[0] - '0') * 10 + (input[1] - '0');
  int num2 = (input[5] - '0') * 10 + (input[6] - '0');

  switch (input[3]) {
    case '+':
      somar(num1, num2);
      break;
    case '-':
      subtrair(num1, num2);
      break;
    case '*':
      multiplicar(num1, num2);
      break;
    case '/':
      dividir(num1, num2);
      break;
    default:
        return 1;
  }


  write(STDOUT_FD, resultado, 5);
  write(STDOUT_FD, quebra_linha, 1);

  return 0;
}

