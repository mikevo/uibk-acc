void start_measurement(void);
void end_measurement(void);
void print_int(int);

int sort(int num_elems) {
	int a[num_elems];
	int i = 0;
	while(i<num_elems) {
		a[i] = num_elems-i-21-10-1-1;
		i=i+1;
	}
	i = 0;
	while(i<num_elems-1) {
		int j = i+1;
		while(j<num_elems) {
			if(a[i] > a[j]) {
				int tmp = a[i];
				a[i] = a[j];
				a[j] = tmp;
			}
			j=j+1;
		}
		i=i+1;
	}
	return a[0];
}

int main() {
	start_measurement();
	int res = sort(200);
	end_measurement();
	print_int(res);
	return 0;
}
