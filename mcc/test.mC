void bar(int arg1);
    
void foo(int arg1, int arg2) {
  if(arg1 < 0) return;
  bar(arg1 - 1);
}

void bar(int arg1) {
  if(arg1 < 0) return;
  foo(arg1 - 1, arg1);
  bar(arg1 - 1);
}   
  
int main() {
   foo(10, 10);
   return 0;
}
