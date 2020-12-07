#include<iostream>
#include<cstring>
using namespace std;
class Matrix
{
public:
    double val[128][128];
    Matrix()
    {
        memset(val, 0, sizeof(val));
    }
    Matrix operator*(const Matrix &b)
    {
        Matrix res;
        for (int i = 0; i < 128; i++)
            for (int j = 0; j < 128; j++)
                for (int u = 0; u < 128; u++)
                    res.val[i][j] = (res.val[i][j] + val[i][u] * b.val[u][j]);
        return res;
    }
    Matrix operator=(const Matrix &b)
    {
        Matrix res;
        for (int i = 0; i < 128; i++)
            for (int j = 0; j < 128; j++)
                res.val[i][j] = val[i][j];
        return res;
    }
};

int main()
{   
    cout << "sb";
    string txt = "3.1415926535282";
    
    int len = txt.length();
    int count[128];
    double mat[128][128];
    memset(count, 0, sizeof(count));
    memset(mat, 0, sizeof(mat));
    cout << "sb";
    Matrix p1, n1, p2, n2, p3, n3;

    count[(int)txt[0]]++;
    for (int i = 1; i < len; i++)
    {
        count[(int)txt[i]]++;
        mat[(int)txt[i - 1]][(int)txt[i]]++;
    }

    for (int i = 0; i < 128; i++)
    {
        int sum1 = 0, sum2 = 0;
        for (int j = 0; j < 128; j++)
        {
            sum1 += mat[i][j];
            sum2 += mat[j][i];
        }
        for (int k = 0; k < 128; k++)
        {
            if (sum1 != 0)
            {
                p1.val[i][k] = mat[i][k] / sum1;
                //if(mat[i][k] != 0)
                //cout<<"i:"<<i<<" k:"<<k<<" val:"<<p1.val[i][k]<<" mat:"<<mat[i][k]<<" sum1:"<<sum1<<endl;
            }
            if (sum2 != 0)
            {
                n1.val[i][k] = mat[i][k] / sum2;
            }
        }
    }
    p2 = p1 * p1;
    p3 = p2 * p1;
    n2 = n1 * n1;
    n3 = n2 * n1;



    cout << "sb";
    return 0;
}