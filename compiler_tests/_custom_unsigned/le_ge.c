int f(unsigned a, unsigned b){
	int z = a <= b;
	z = z + (b >= a);
	return z;
}
