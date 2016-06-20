void start_measurement(void);
void end_measurement(void);
void print_float(float);

float dot_product(int num_elems) {
	float a[num_elems];
	float b[num_elems];
	float init = 2.0;
	int i = 0;
	while(i<num_elems) {
		a[i] = init;
		b[i] = 2.0;
		init = init + 1.0;
		i=i+1;
	}
	float res = 0.0;
	i = 0;
	while(i<num_elems) {
		res = res + (a[i]*b[i]);
		i=i+1;
	}
	return res;
}

int main() {
	start_measurement();
	float res = dot_product(1000);
	end_measurement();
	print_float(res);
	return 0;
}
