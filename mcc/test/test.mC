void bar(int arg1);
  
void foo(int arg1) {
   if(arg1 < 0) return;
   bar(arg1 - 1);
}

void bar(int arg1) {
   if(arg1 < 0) return;
   foo(arg1 - 1);
   bar(arg1 - 1);
}   

 int main() {
    float a = 10.0;
    float b = 10.0;
    float c = a + b;
    return 0;
 }