void print_int(int);

int foo(int arg1, int arg2);
int bar(int arg1, int arg2);

int bar(int arg1, int arg2) {
    print_int(2);

    if(arg1 > 5) {
        arg1 = arg1 - 1;
    } else {
      return 3;
    }

    return foo(arg1, arg2);
}

int foo(int arg1, int arg2) {
    print_int(1);

    if(arg1 <= 5) {
        return 42;
    } else {
        return bar(arg2, arg1);
    }
}

int main() {
  int i = 0;
  while(i < 10) {
    print_int(foo(i, 13));
    i = i + 1;
  }

  return 0;
}
