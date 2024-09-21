#include <stdio.h>
int f(int n);

int main()
{
    printf("rec int out: %d\n", f(5));
    return !(f(5)==15);
}
