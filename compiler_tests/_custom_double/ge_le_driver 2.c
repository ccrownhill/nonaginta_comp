#include <stdio.h>
int f();

int main()
{
	int x = f();
	//printf("%d\n", x);
	if (x ==5) return 1;
	if (x == 0) return 1;
	if (x == 2) return 0;
	if (x == 1) return 1;
	if (x == 7) return 1;

	
}