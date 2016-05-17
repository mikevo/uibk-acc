void print_int(int);
void print_float(float);

float foo(int arg1, float arg2) {
    if(arg1 <= 5) {
        return 42.0;
    } else {
        return arg2;
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