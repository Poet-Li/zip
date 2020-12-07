#include<iostream>

int main()
{   
    int sum = 0;
    for (int i = 0; i < 128; i++)
            for (int j = 0; j < 128; j++)
                for (int u = 0; u < 128; u++)
                    sum++;
    std::cout << sum;
    return 0;
}