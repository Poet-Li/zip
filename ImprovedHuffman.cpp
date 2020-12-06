#include <iostream>
#include <vector>
#include <string>
#include <map>
#define MAX 100001
#define MAXWINDOW 3
using namespace std;
class matrix
{
public:
    int size;
    double **val;
    matrix(int size = 1)
    {
        this->size = size;
        val = (double **)malloc(size * size * sizeof(double));
    }
    ~matrix()
    {
        if (val != NULL)
            delete val;
    }
};
class Solution
{
public:
    //data
    string originalStr; //初始字符集
    int len;            //字符集长度
    double compressionRate;

    double forwardtemp1[MAX];    //一阶向前转移概率阈值
    double backtemp1[MAX];       //一阶向后转移概率阈值
    double forwardtempn[MAX][5]; //多阶向前转移概率阈值，根据具体的字符串计算得到
    double backtempn[MAX][5];    //多阶向后转移概率阈值

    matrix p1; //一阶向前转移矩阵
    matrix n1; //一阶向后转移矩阵
    matrix p2; //二阶向前转移矩阵
    matrix n2; //二阶向后转移矩阵
    matrix p3; //三阶向前转移矩阵
    matrix n3; //三阶向后转移矩阵

    map<string, int> stringset;      //字符子串与权重的一一映射
    map<string, string> huffmanCode; //字符子串与编码的一一映射
    string Strcode;                  //整个字符串对应的编码集
    string decodeStr;                //解码后的字符串
    //func
    void readFlie(); //将txt文件读入originalStr中

    void statistics(); //数据统计模块，用originalStr计算出一二三阶的向前向后转移阈值与转移矩阵（共6个值，6个矩阵需要计算）

    void calculatetemp1(); //计算阈值

    void calculatetempn(); //计算n阶阈值

    void judgeRelation(); //相关性判断模块，由originalStr和转移矩阵构建stringset

    void bulidTree(); //由stringset构建哈夫曼树，得到数据huffmanCode

    void encode(); //压缩编码,由huffmanCode得到Strcode

    void decode(); //解压缩，由huffmanCode和Strcode得到decodeStr

    void countRate(); //计算压缩率
    void solve()
    {
        readFlie();
        statistics();
        judgeRelation();
        bulidTree();
        encode();
        decode();
        countRate();
    }
};

void Solution::calculatetemp1()
{
    for (int i = 0; i < p1.size; i++)
    {
        double cnt = 0;
        double m = MAX;
        for (int j = 0; j < p1.size; j++)
        {
            cnt += p1.val[i][j];
            m = min(m, p1.val[i][j]);
            if (cnt >= 0.8)
            {
                break;
            }
        }
        forwardtemp1[i] = m;
    }
    for (int i = 0; i < p1.size; i++)
    {
        double cnt = 0;
        double m = MAX;
        for (int j = 0; j < p1.size; j++)
        {
            cnt += n1.val[i][j];
            m = min(m, n1.val[i][j]);
            if (cnt >= 0.8)
            {
                break;
            }
        }
        backtemp1[i] = m;
    }
}

void Solution::calculatetempn()
{
    for (int i = 0; i < originalStr.size(); i++)
    {
        int k = originalStr[i];
        double ans = forwardtemp1[k];
        int j = i + 1;
        for (; j < p1.size && j - i <= 3; j++)
        {
            forwardtempn[i][j - i] = ans;
            ans *= forwardtemp1[j];
        }
    }
}

void Solution::judgeRelation()
{
    int posi = 0;
    int step;
    int flag = 0;
    calculatetemp1();
    calculatetempn();
    while (posi < len - 3)
    {
        for (int i = posi; i < MAXWINDOW; i++)
        {
            if (p1.val[i][i + step] >= forwardtemp1[i] && n1.val[i][i + step] >= backtemp1[i])
            {
                flag = i;
                continue;
            }
            else
            {
                flag = i - 1;
                break;
            }
        }

        //一阶已经完成
        step = posi + 2;
        //flag是滑动窗口的右边界
        if (flag - posi > 1) //零阶直接跳出
        {
            int l = flag;
            for (int j = step; j <= l; j++)
            {
                if (j - posi == 2)
                    if (p2.val[posi][j] >= forwardtempn[posi][j - posi] && n2.val[posi][j] >= backtempn[posi][j - posi])
                    {
                        flag = j;
                        continue;
                    }
                    else
                    {
                        flag = j - 1;
                        break;
                    }
                if (j - posi == 3)
                {
                    if (p3.val[posi][j] >= forwardtempn[posi][j - posi] && n3.val[posi][j] >= backtempn[posi][j - posi])
                    {
                        flag = j;
                        continue;
                    }
                    else
                    {
                        flag = j - 1;
                        break;
                    }
                }
            }
        }
        string s = originalStr.substr(posi, flag + 1 - posi);

        pair<map<string, int>::iterator, bool> Insert_Pair;

        Insert_Pair = stringset.insert(map<string, int>::value_type(s, 1));

        if (!Insert_Pair.second)
        {
            stringset[s]++;
        }
        posi += s.size();
    }
}
void Solution::countRate()
{
    compressionRate = Strcode.size() / (originalStr.size() * 8);
    cout << "The compression rate is " << compressionRate << endl;
}
int main()
{
    Solution s;
    s.solve();
}
