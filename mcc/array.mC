void print_int(int out);

int main(void) {
  int a[5];
  int b[5];
  int c[5];

  int i = 0;

  while (i <= 5) {
    a[i] = i + 1;
    b[i] = i + 2;
    i = i + 1;
  }

  i = 0;
  while (i < 5) {
    c[i] = a[i] * b[i];
    i = i + 1;
  }

  int sum = 0;
  i = 0;
  while (i < 5) {
    sum = sum + c[i];
    i = i + 1;
  }

  print_int(sum);

  return 0;
}
