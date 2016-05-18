void print_int(int);
void print_float(float);

float foo(int arg1, float arg2);
float bar(float arg1, int arg2);

float bar(float arg1, int arg2) {
    print_int(2);

    if(arg1 > 5.0) {
        arg1 = arg1 - 1.0;
    }

    return foo(arg1, arg2);
}

float foo(int arg1, float arg2) {
    print_int(1);

    if(arg1 <= 5) {
        return 42.0;
    } else {
        return bar(arg2, arg1);
    }
}

int main() {
  int i = 0;
  while(i < 10) {
    print_float(foo(i, 13.2));
    i = i + 1;
  }

  return 0;
}