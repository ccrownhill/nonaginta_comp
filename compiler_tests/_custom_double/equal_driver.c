#include <stdio.h>
int f(double x, double y);

int main()
{
    if (!(f(3.3, 3.3))) {
		printf("Test failed for x = 3.3, y = 3.3\n");
		return 1;
		
	}
    if (f(3.3, 2.2)) {
		printf("Test failed for x = 3.3, y = 2.2\n");
		return 1;
		
	}
    return 0;
}