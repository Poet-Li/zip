#define MAX 100001
#define MAXWINDOW 3
#include <bits/stdc++.h>
using namespace std;
class Matrix
{
public:
    double val[128][128];
    Matrix()
    {
        memset(val, 0, sizeof(val));
    }
    Matrix operator*(const Matrix b)
    {
        Matrix res;
        for (int i = 0; i < 128; i++)
            for (int j = 0; j < 128; j++)
                for (int u = 0; u < 128; u++)
                    res.val[i][j] = (res.val[i][j] + val[i][u] * b.val[u][j]);
        return res;
    }
    Matrix operator=(const Matrix b)
    {
        Matrix res;
        for (int i = 0; i < 128; i++)
            for (int j = 0; j < 128; j++)
                res.val[i][j] = val[i][j];
        return res;
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

    Matrix p1; //一阶向前转移矩阵
    Matrix n1; //一阶向后转移矩阵
    Matrix p2; //二阶向前转移矩阵
    Matrix n2; //二阶向后转移矩阵
    Matrix p3; //三阶向前转移矩阵
    Matrix n3; //三阶向后转移矩阵

    map<string, int> stringset;      //字符子串与权重的一一映射
    map<string, string> huffmanCode; //字符子串与编码的一一映射
    string Strcode;                  //整个字符串对应的编码集
    string decodeStr;                //解码后的字符串
    //func

    //将txt文件读入originalStr中
    string readFlie(string filePath);

    //数据统计模块，用originalStr计算出一二三阶的向前向后转移阈值与转移矩阵（共6个值，6个矩阵需要计算）
    void statistics(string txt);

    void calculatetemp1(); //计算阈值

    void calculatetempn(); //计算n阶阈值

    void writeBinaryFile(string huffcode, string fileToWrite);

    void judgeRelation();

    void buildTree(); //由stringset构建哈夫曼树，得到数据huffmanCode

    void encode(); //压缩编码,由huffmanCode得到Strcode

    void decode(); //解压缩，由huffmanCode和Strcode得到decodeStr

    void countRate(); //计算压缩率

    // 我个人不建议用这个 不如一个一个函数调用
    /*void solve()
    void countRate(); //计算压缩率
    void solve()
    {
        readFlie();
        statistics();
        judgeRelation();
        buildTree();
        encode();
        decode();
        countRate();
    }*/
};

string Solution::readFlie(string filePath)
{
    string ans;
    ifstream infile;
    infile.open(filePath.data());
    assert(infile.is_open());
    char c;
    infile >> noskipws;
    while (!infile.eof())
    {
        infile >> c;
        //cout<<c<<endl;
        ans.push_back(c);
    }
    infile.close();
    return ans;
}

void Solution::statistics(string txt)
{
    int len = txt.length();
    int count[128];
    double mat[128][128];
    memset(count, 0, sizeof(count));
    memset(mat, 0, sizeof(mat));

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

    return;
}

void Solution::writeBinaryFile(string huffcode, string fileToWrite)
{
    ofstream outFile(fileToWrite.c_str(), ios::binary | ios::out); //以二进制写模式打开文件
    if (!outFile)
    {
        cout << "New file open error." << endl;
        return;
    }
    int wlen = 8;
    int i;
    for (i = 0; i < huffcode.length(); i = i + wlen)
    {
        char *p = (char *)malloc(wlen * sizeof(char));
        string sub = huffcode.substr(i, i + wlen);
        bitset<8> bit(sub);
        char a = bit.to_ulong(); //这里为0-256
        outFile.put(a);
        free(p);
    }
    outFile.close();
}

int main(int argc, char **argv)
{
    Solution s;
    //s.solve();
}
void Solution::calculatetemp1()
{
    for (int i = 0; i < 128; i++)
    {
        double cnt = 0;
        double m = MAX;
        for (int j = 0; j < 128; j++)
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
    for (int i = 0; i < 128; i++)
    {
        double cnt = 0;
        double m = MAX;
        for (int j = 0; j < 128; j++)
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
        for (; j < 128 && j - i <= 3; j++)
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
}
