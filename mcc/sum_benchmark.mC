void print_int(int out);
int read_int();

int sumUpTo(int n) {
  int i = 1;
  int sum = 0;

  while (i <= n) {
    sum = sum + i;
    i = i + 1;
  }

  return sum;
}

int main(void) {
  int sum = sumUpTo(110000000);
  print_int(sum);

  return 0;
}
