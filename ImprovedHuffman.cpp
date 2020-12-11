#define MAX 8000
#define MAXWINDOW 8
#define yuzhi 0.5
#define Longest 9 //最长的字符字串的长度
#include <algorithm>
#include <bitset>
#include <cassert>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <vector>
using namespace std;
typedef pair<string, int> node;

class Matrix
{
public:
    double val[128][128];
    Matrix() { memset(val, 0, sizeof(val)); }
    Matrix operator*(const Matrix &b)
    {
        Matrix res;
        for (int i = 0; i < 128; i++)
            for (int j = 0; j < 128; j++)
                for (int k = 0; k < 128; k++)
                {
                    res.val[i][j] = (res.val[i][j] + val[i][k] * b.val[k][j]);
                }
        return res;
    }
    Matrix &operator=(const Matrix &b)
    {
        for (int i = 0; i < 128; i++)
            for (int j = 0; j < 128; j++)
                val[i][j] = b.val[i][j];
        return *this;
    }
};

double getval(Matrix a, Matrix b, int i, int j)
{
    double ans = 0;
    for (int k = 0; k < 128; k++)
    {
        ans = (ans + a.val[i][k] * b.val[k][j]);
    }
    return ans;
}

class TreeNode
{
public:
    node Node;
    TreeNode *left;
    TreeNode *right;
    string huffcode;
};

struct compare
{ //用于优先队列的比较
    bool operator()(TreeNode *&a, TreeNode *&b)
    {
        return a->Node.second > b->Node.second;
    }
};

class Solution
{
public:
    // data
    string originalStr; //初始字符集
    int len;            //字符集长度
    double compressionRate;

    double forwardtemp1[128]; //一阶向前转移概率阈值
    double backtemp1[128];    //一阶向后转移概率阈值

    Matrix p1; //一阶向前转移矩阵
    Matrix n1; //一阶向后转移矩阵
    Matrix p2; //二阶向前转移矩阵
    Matrix n2; //二阶向后转移矩阵
    Matrix p3; //三阶向前转移矩阵
    Matrix n3; //三阶向后转移矩阵
    Matrix p4; //三阶向前转移矩阵
    Matrix n4; //三阶向后转移矩阵

    map<string, int> stringset;      //字符子串与权重的一一映射
    map<string, string> huffmanCode; //字符子串与编码的一一映射
    map<string, string> CodeToWord;  //编码到字符字串的映射
    // string originalStr;                  //整个字符串对应的编码集
    string StrOf05;   //用字符串存的05串嗷
    string huffTable; //映射表

    string Str_File_Connected; //治达用这个string来做文件操作就行
    // func

    //将txt文件读入originalStr中
    void readFlie(string filePath);

    //数据统计模块，用originalStr计算出一二三阶的向前向后转移阈值与转移矩阵（共6个值，6个矩阵需要计算）
    void statistics(string &txt);

    void calculatetemp1(); //计算阈值

    double getforwardtempn(int str, int sq);

    double getbackwardtempn(int str, int sq);

    void writeBinaryFile(string filePath); // 把字符串写进txt文件中

    void judgeRelation();

    //调用下面那个buildTree函数就行，这个是buildTree中要用到的一个递归函数
    void getHuffmanCode(TreeNode *treenode, string code);

    void buildTree(); //由stringset构建哈夫曼树，得到数据huffmanCode

    void encode(); //压缩编码,由huffmanCode得到originalStr

    void decode(); //解压缩，由huffmanCode和originalStr得到decodeStr

    void countRate(); //计算压缩率

    void readTxtToString(string filePath);

    void writeStringToTxt(string filePath);
};

void Solution::readFlie(string filePath)
{
    ifstream infile;
    infile.open(filePath.c_str());
    assert(infile.is_open());
    char c;
    infile >> noskipws;
    while (infile.peek() != EOF)
    {
        infile.get(c);
        // cout<<c<<endl;
        originalStr.push_back(c);
    }
    infile.close();
}

void Solution::statistics(string &txt)
{
    len = txt.length();
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
                p1.val[i][k] = (mat[i][k] / sum1);
                // if(mat[i][k] != 0)
                // cout<<"i:"<<i<<" k:"<<k<<" val:"<<p1.val[i][k]<<"
                // mat:"<<mat[i][k]<<" sum1:"<<sum1<<endl;
            }
            if (sum2 != 0)
            {
                n1.val[i][k] = (mat[i][k] / sum2);
            }
        }
    }

    p2 = p1 * p1;
    p3 = p2 * p1;
    p4 = p2 * p2;
    n2 = n1 * n1;
    n3 = n2 * n1;
    n4 = n2 * n2;
    return;
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
            if (p1.val[i][j] > 0)
                m = min(m, p1.val[i][j]);
            if (cnt >= yuzhi)
            {
                break;
            }
        }
        if (m == MAX)
            m = 0;
        forwardtemp1[i] = m;
    }
    for (int i = 0; i < 128; i++)
    {
        double cnt = 0;
        double m = MAX;
        for (int j = 0; j < 128; j++)
        {
            cnt += n1.val[i][j];
            if (n1.val[i][j] > 0)
                m = min(m, n1.val[i][j]);
            if (cnt >= yuzhi)
            {
                break;
            }
        }
        if (m == MAX)
            m = 0;
        backtemp1[i] = m;
    }
}

inline double Solution::getforwardtempn(int str, int sq)
{
    int c = originalStr[str];
    double ans = forwardtemp1[c];
    for (int i = str + 1; i <= str + sq; i++)
    {
        c = originalStr[i];
        ans *= forwardtemp1[c];
    }
    return ans;
}

inline double Solution::getbackwardtempn(int str, int sq)
{
    int c = originalStr[str];
    double ans = backtemp1[c];
    for (int i = str + 1; i <= str + sq; i++)
    {
        c = originalStr[i];
        ans *= backtemp1[c];
    }
    return ans;
}

void Solution::judgeRelation()
{
    int posi = 0;
    int step;
    int flag = 0;
    calculatetemp1();
    // for (int i = 0; i < 128; i++) {
    //     char k = i;
    //     string s(1, k);
    //     stringset.insert(map<string, int>::value_type(s, 0));
    // }
    while (posi < len)
    {
        step = 1;
        for (int i = posi; i < MAXWINDOW + posi; i++)
        {
            int p = originalStr[i];
            int w = originalStr[i + step];
            if (p1.val[p][w] > forwardtemp1[p] && n1.val[p][w] > backtemp1[p])
            {
                flag = i + step;
                continue;
            }
            else
            {
                flag = i;
                break;
            }
        }

        //一阶已经完成
        step = posi + 2;
        // flag是滑动窗口的右边界
        if (flag - posi > 1) //零阶直接跳出
        {
            int l = flag;
            for (int j = step; j <= l; j++)
            {
                if (j - posi == 2)
                {
                    int p = originalStr[posi];
                    int b = originalStr[posi + 2];
                    if (p2.val[p][b] > getforwardtempn(posi, 2) &&
                        n2.val[p][b] > getbackwardtempn(posi, 2))
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
                else if (j - posi == 3)
                {
                    {
                        int p = originalStr[posi];
                        int b = originalStr[posi + 3];
                        if (p3.val[p][b] > getforwardtempn(posi, 3) &&
                            n3.val[p][b] > getbackwardtempn(posi, 3))
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
                else if (j - posi == 4)
                {
                    {
                        int p = originalStr[posi];
                        int b = originalStr[posi + 4];
                        if (p4.val[p][b] > getforwardtempn(posi, 4) &&
                            n4.val[p][b] > getbackwardtempn(posi, 4))
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
                else if (j - posi == 5)
                {
                    {
                        int p = originalStr[posi];
                        int b = originalStr[posi + 5];
                        if (getval(p2, p3, p, b) > getforwardtempn(posi, 5) &&
                            getval(n2, n3, p, b) > getbackwardtempn(posi, 5))
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
                else if (j - posi == 6)
                {
                    {
                        int p = originalStr[posi];
                        int b = originalStr[posi + 6];
                        if (getval(p3, p3, p, b) > getforwardtempn(posi, 6) &&
                            getval(n3, n3, p, b) > getbackwardtempn(posi, 6))
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
                else if (j - posi == 7)
                {
                    {
                        int p = originalStr[posi];
                        int b = originalStr[posi + 7];
                        if (getval(p4, p3, p, b) > getforwardtempn(posi, 7) &&
                            getval(n4, n3, p, b) > getbackwardtempn(posi, 7))
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
                else if (j - posi == 8)
                {
                    {
                        int p = originalStr[posi];
                        int b = originalStr[posi + 7];
                        if (getval(p4, p4, p, b) > getforwardtempn(posi, 8) &&
                            getval(n4, n4, p, b) > getbackwardtempn(posi, 8))
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
        }
        string s = originalStr.substr(posi, flag + 1 - posi);

        pair<map<string, int>::iterator, bool> Insert_Pair;

        Insert_Pair = stringset.insert(map<string, int>::value_type(s, 1));

        if (!Insert_Pair.second)
        {
            stringset[s]++;
        }
        posi += s.size();
        // cout << posi << endl;
    }
    // cout << cnt << endl;
}
void Solution::countRate()
{
    double a = StrOf05.size() + huffTable.size();
    double b = len;
    compressionRate = a / len / 8;
    // cout << a << endl << b << endl;
    cout << "Successfully compact! The compact rate is " << compressionRate
         << endl;
}

void Solution::getHuffmanCode(TreeNode *treenode, string code)
{
    treenode->huffcode = code;
    if (treenode->left != NULL)
        getHuffmanCode(treenode->left, code + "0");
    if (treenode->right != NULL)
        getHuffmanCode(treenode->right, code + "1");
    if ((treenode->right == NULL) && (treenode->left == NULL))
    {
        huffmanCode[treenode->Node.first] = treenode->huffcode;
        CodeToWord[treenode->huffcode] = treenode->Node.first;
    }
}

void Solution::buildTree()
{
    //由stringset构建哈夫曼树，然后调用getHuffmanCode()
    vector<node> Vec(stringset.begin(), stringset.end());
    int KeyNumber = Vec.size();
    priority_queue<TreeNode *, vector<TreeNode *>, compare> Queue;
    for (int i = 0; i < KeyNumber; i++)
    {
        TreeNode *tempnode = new TreeNode();
        tempnode->left = NULL;
        tempnode->right = NULL;
        tempnode->Node = Vec.back();
        Vec.pop_back();
        Queue.push(tempnode);
    } //以上为将stringset中的内容转化为哈夫曼树的节点并存入优先队列

    TreeNode *node1 = new TreeNode();
    TreeNode *node2 = new TreeNode();

    for (int i = 0; i < KeyNumber - 1; i++)
    {
        node1 = Queue.top();
        Queue.pop();

        node2 = Queue.top();
        Queue.pop();

        TreeNode *Newnode = new TreeNode();
        Newnode->left = node1;
        Newnode->right = node2;
        Newnode->Node.second = node1->Node.second + node2->Node.second;

        Queue.push(Newnode);
    } //构造哈夫曼树，得到根节点root
    TreeNode *root = Queue.top();
    getHuffmanCode(root, "");
    delete node1;
    delete node2;
}

void Solution::encode()
{ //如果字符字串有/t开头的可能会出问题？
    int TempLength = Longest;
    //设置一个TempLength变量，初始为最长长度，每次先检测当前字符串头的前TempLength个字符
    //之和，看看是不是map中对应的一个单词
    while (originalStr.length() > Longest)
    {
        map<string, string>::iterator iter =
            huffmanCode.find(originalStr.substr(0, TempLength));
        if (iter != huffmanCode.end())
        {
            StrOf05 += iter->second;
            originalStr.erase(originalStr.begin(),
                              originalStr.begin() + TempLength);
            TempLength = Longest;
            continue;
        }
        TempLength--;
        if (TempLength == 0)
        {
            cout << "fail" << endl;
        }
    }

    // cout<<huffmanCode.find("00")->second<<endl;

    // cout<<"!"<<originalStr<<endl;
    // cout<<StrOf05.substr(StrOf05.size()-30, 30)<<endl;
    //处理屁股
    // cout << "-----" << endl;
    TempLength = originalStr.length();
    while (!originalStr.empty())
    {
        map<string, string>::iterator iter =
            huffmanCode.find(originalStr.substr(0, TempLength));
        if (iter != huffmanCode.end())
        {
            StrOf05 += iter->second;
            originalStr.erase(originalStr.begin(),
                              originalStr.begin() + TempLength);
            TempLength = originalStr.length();
            continue;
        }
        TempLength--;
        if (TempLength == 0)
        {
            cout << "fail" << endl;
        }
    }
    // cout<<"!"<<originalStr<<endl;
    // cout<<StrOf05.substr(StrOf05.size()-30, 30)<<endl;

    // 补全八位
    // cout<<StrOf05.size() % 8 << endl;
    // cout<<StrOf05.substr(StrOf05.size()-20,20)<<endl;
    // int num = StrOf05.length() % 8;
    // StrOf05.insert(int(StrOf05.length() / 8) * 8, 8-num, '0');
    // cout<<StrOf05.substr(StrOf05.size()-20,20)<<endl;
    // cout<<StrOf05.size() % 8 << endl;

    // string huffTable; //储存map部分
    huffTable.clear();
    for (map<string, int>::iterator iter = stringset.begin();
         iter != stringset.end(); iter++)
    {
        huffTable += "\a";
        huffTable += iter->first;
        huffTable += "\a";
        huffTable += to_string(iter->second);
    }
    huffTable += "\a";
    huffTable += "\t";
    Str_File_Connected = StrOf05 + huffTable;
}

void Solution::writeBinaryFile(string fileToWrite)
{
    ofstream outFile(fileToWrite.c_str(),
                     ios::binary | ios::out); //以二进制写模式打开文件
    if (!outFile)
    {
        cout << "New file open error." << endl;
        return;
    }
    // 写进05串的长度
    string len = to_string(StrOf05.length());
    outFile.write(len.c_str(), len.length());
    outFile.put('\n');

    int wlen = 8;
    int i;
    for (i = 0; i <= StrOf05.length() - 8; i = i + wlen)
    {
        string sub = StrOf05.substr(i, wlen);
        bitset<8> bit(sub);
        char a = bit.to_ulong(); //这里为0-256
        outFile.put(a);
    }
    // i -= wlen;
    // i += 1; //???????????????????????????????我日？？？？？？？？？？？？
    while (i < StrOf05.length())
    {
        outFile.put(StrOf05[i]);
        i++;
    }

    for (i = 0; i < huffTable.length(); i++)
    {
        outFile.put(huffTable[i]);
    }
    // cout << count(Str_File_Connected.begin(), Str_File_Connected.end(),
    // '\a')<<endl; cout << count(huffTable.begin(), huffTable.end(),
    // '\a')<<endl;
    outFile.close();
}
void Solution::decode()
{
    //得到StrOf05部分
    int temp = 0;
    StrOf05.clear();
    while (Str_File_Connected.at(temp) != '\a')
        temp++;
    StrOf05 = Str_File_Connected.substr(0, temp);
    Str_File_Connected.erase(Str_File_Connected.begin(),
                             Str_File_Connected.begin() + temp + 1);
    // cout << StrOf05.length() << endl;

    //载入map部分
    stringset.clear();
    temp = 0;
    int tempbegin = 0;
    while (Str_File_Connected.at(temp) != '\t')
    {
        while (Str_File_Connected.at(temp) != '\a')
        {
            temp++;
        }
        string first = Str_File_Connected.substr(tempbegin, temp - tempbegin);
        temp++;
        tempbegin = temp;
        while (Str_File_Connected.at(temp) != '\a')
        {
            temp++;
        }
        int second =
            stoi(Str_File_Connected.substr(tempbegin, temp - tempbegin));
        temp++;
        tempbegin = temp;
        stringset.insert(make_pair(first, second));
    }
    // cout << "fdsfsdf" << endl;
    // cout<<stringset.find("00")->second <<endl;

    //通过载入的stringset得到CodeToWord
    buildTree();
    // cout<<huffmanCode.find("00")->second<< "---" <<endl;
    // if(CodeToWord.find("050050000")==CodeToWord.end())
    // cout<<"nonexist"<<endl;
    //翻译部分
    int TempLength = 0;
    Str_File_Connected.clear();
    while (!StrOf05.empty())
    {
        map<string, string>::iterator iter =
            CodeToWord.find(StrOf05.substr(0, TempLength));
        if (iter != CodeToWord.end())
        {
            Str_File_Connected += iter->second;
            // cout<<iter->second<<endl;
            StrOf05.erase(StrOf05.begin(), StrOf05.begin() + TempLength);
            TempLength = 0;
            continue;
        }
        TempLength++;
        // if(CodeToWord.find("00")==CodeToWord.end()) cout<<"nonexist"<<endl;
        // if (TempLength > 10) cout << "ddddd" << endl;
        // cout<<StrOf05.length()<<endl;
        // if (StrOf05.size() < 100) {
        //     cout << StrOf05 << endl;
        // }
    }
}

string convert(int a)
{
    string ans;
    for (int i = 0; i < 8; i++)
    {
        int p = pow(2, 7 - i);
        ans.push_back('0' + a / p);
        a -= p;
        if (a < 0)
            a += p;
    }
    return ans;
}

void Solution::writeStringToTxt(string filePath)
{
    ofstream o(filePath);
    o.write(Str_File_Connected.c_str(), Str_File_Connected.length());
    o.close();
}

void Solution::readTxtToString(string filePath)
{
    ifstream infile;
    infile.open(filePath.c_str(), ios::binary | ios::in);
    assert(infile.is_open());

    char c;
    // int cnt = 0;
    infile >> noskipws;
    infile.get(c);

    string l;
    while (c != '\n')
    {
        l.push_back(c);
        infile.get(c);
    }

    // cout << l << endl;
    // cout<<"c:"<<(int)c<<endl;
    int len = stoi(l);
    len /= 8;
    int cnt = 0;
    while (cnt < len)
    {
        infile.get(c);
        int c1 = (c < 0) ? c + 256 : c;
        string c2 = convert(c1);
        // cout<<c2<<endl;
        Str_File_Connected.append(c2);
        cnt++;
    }

    // infile.get(c);

    // while (c != '\a') {
    //     infile.get(c);
    //     cout << "c: " << (int)c << endl;
    //     Str_File_Connected.push_back(c);
    // }
    // cout<<Str_File_Connected[19627]<<endl;
    // cout << Str_File_Connected.length() << endl;

    // cout << "-----输入map-----" << endl;
    // infile.get(c); // ？？？？？？？？？？？？
    while (infile.peek() != EOF)
    {
        infile.get(c);
        Str_File_Connected.push_back(c);
        // cout<<"c: "<<(int)c<<endl;
    }
    infile.close();
}

int main(int argc, char *argv[])
{
    cout
        << "The usage of this program: " << endl
        << "1. compact:    ziptxt   {original filename}  {compacted filename}"
        << endl
        << "2. uncompact:  unziptxt {compacted filename} {uncompacted filename}"
        << endl
        << "3. Enter ctrl+z to quit." << endl
        << endl;
    string a, b, c;
    while (cin >> a >> b >> c)
    {
        if (a[0] == 'z')
        {
            cout << "Compacting..." << endl;
            Solution s;
            s.readFlie(b);
            // cout << 1;
            s.statistics(s.originalStr);
            // cout << 2;
            s.judgeRelation();
            // cout << 3;
            s.buildTree();
            // cout << 4;
            s.encode();
            // cout << 5;
            s.writeBinaryFile(c);
            // cout << 6;
            s.countRate();
            // cout << 7;
        }
        else if (a[0] == 'u')
        {
            cout << "Uncompacting..." << endl;
            Solution p;
            p.readTxtToString(b); // Str_FILE
            p.decode();
            p.writeStringToTxt(c);
            cout << "Sucessfully uncompact!" << endl;
        }
    }
    return 0;
}
