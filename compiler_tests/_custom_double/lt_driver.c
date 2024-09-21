int f(double x, double y);

int main()
{
    if (!(f(3.3, 3.5))) return 1;
    if (f(3.2, 3.2)) return 1;
    if (f(3.2001, 3.2)) return 1;
    return 0;
}