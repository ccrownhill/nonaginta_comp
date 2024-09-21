int f(float x, float y);

int main()
{
    if (!(f(3.3, 3.3))) return 1;
    if (f(3.3, 2.2)) return 1;
    return 0;
}