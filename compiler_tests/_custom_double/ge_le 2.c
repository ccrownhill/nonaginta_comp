int f(){
	double x = 3.3;
	double y = 2.3;
	int z = 0;
	if (x <= y) z=z+1 ;
	if (y >= x) z=z+1 ;
	if (x >= x) return z;
	else
		return 5;
}