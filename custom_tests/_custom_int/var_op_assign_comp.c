int f()
{
	int x = 3;
	int y = 9;
	int z = 1;
	y += x; 
	y -= x; 
	y *= x; 
	y /= x; 
	y %= x; 
	z <<= 1; 
	z >>= 1; 
	z &= 1; 
	z |= 1;  
	z ^= 1; 


	return y + z;
}