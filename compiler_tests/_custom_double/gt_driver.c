int f(double x, double y);

int main()
{
    if (!(f(3.5, 3.3))) return 1;
    if (f(3.2, 3.2)) return 1;
    if (f(3.2, 3.2001)) return 1;
    return 0;
}