typedef int *int_t;

int g(int y)
{
    int_t x = &y;
    return *x;
}