int foo(int bar) {
    int a = bar;

    if (a < 10) {
        return a;
    } else {
        return foo(a);
    }
}

int main() {
    foo(42);
    return 0;
}