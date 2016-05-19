void print_int(int out);
int read_int();

int fibonacci(int n) {
  if ( n == 0 ) {
    return 0;
  } else if ( n == 1 ) {
    return 1;
  } else {
    return (fibonacci(n-1) + fibonacci(n-2));
  }
}

int main(void) {
  int fib = fibonacci(42);
  print_int(fib);

  return 0;
}
