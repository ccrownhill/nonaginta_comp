int f(float x);

int main()
{
    if( (f(3.3)!=0) ) return 1;
    if( (f(0.0)!=1) ) return 1;
    return 0;
}