#include <stdio.h>

void print_array(int *a, int n)
{
    int i;
    for (i = 0; i < n; i++)
    {
        a[i] = i;
        printf("%d ", a[i]);
    }
    printf("\n");
}

int main(void)
{
    int a[7] = {0};
    print_array(a, 10);

    return 0;
}
