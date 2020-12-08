#define MAX 1001
#define MAXWINDOW 3
#define yuzhi 1
#include <algorithm>
#include <cstring>
#include <iostream>
#include <map>
#include <vector>
#define Longest 4  //最长的字符字串的长度
#include <bits/stdc++.h>
typedef pair<string, int> node;
using namespace std;
class Matrix {
   public:
    double val[128][128];
    Matrix() { memset(val, 0, sizeof(val)); }
    Matrix operator*(const Matrix &b) {
        Matrix res;
        for (int i = 0; i < 128; i++)
            for (int j = 0; j < 128; j++)
                for (int u = 0; u < 128; u++)
                    res.val[i][j] = (res.val[i][j] + val[i][u] * b.val[u][j]);
        return res;
    }
    Matrix operator=(const Matrix &sxedrf) {
        Matrix res;
        for (int i = 0; i < 128; i++)
            for (int j = 0; j < 128; j++) res.val[i][j] = val[i][j];
        return res;
    }
};

class TreeNode {
   public:
    node Node;
    TreeNode *left;
    TreeNode *right;
    string huffcode;
};

struct compare {  //用于优先队列的比较
    bool operator()(TreeNode *&a, TreeNode *&b) {
        return a->Node.second > b->Node.second;
    }
};

class Solution {
   public:
    // data
    string originalStr;  //初始字符集
    int len;             //字符集长度
    double compressionRate;

    double forwardtemp1[MAX];  //一阶向前转移概率阈值
    double backtemp1[MAX];     //一阶向后转移概率阈值
    double forwardtempn[MAX]
                       [5];  //多阶向前转移概率阈值，根据具体的字符串计算得到
    double backtempn[MAX][5];  //多阶向后转移概率阈值

    Matrix p1;  //一阶向前转移矩阵
    Matrix n1;  //一阶向后转移矩阵
    Matrix p2;  //二阶向前转移矩阵
    Matrix n2;  //二阶向后转移矩阵
    Matrix p3;  //三阶向前转移矩阵
    Matrix n3;  //三阶向后转移矩阵

    map<string, int> stringset;       //字符子串与权重的一一映射
    map<string, string> huffmanCode;  //字符子串与编码的一一映射
    map<string, string> CodeToWord;   //编码到字符字串的映射
    string Strcode;                   //整个字符串对应的编码集
    string StrOf01;                   //用字符串存的01串嗷

    string Str_File_Connected;  //治达用这个string来做文件操作就行
    // func

    //将txt文件读入originalStr中
    void readFlie(string filePath);

    //数据统计模块，用originalStr计算出一二三阶的向前向后转移阈值与转移矩阵（共6个值，6个矩阵需要计算）
    void statistics(string &txt);

    void calculatetemp1();  //计算阈值

    void calculatetempn();  //计算n阶阈值

    void writeBinaryFile(string huffcode, string fileToWrite);

    void judgeRelation();

    //调用下面那个buildTree函数就行，这个是buildTree中要用到的一个递归函数
    void getHuffmanCode(TreeNode *treenode, string code);

    void buildTree();  //由stringset构建哈夫曼树，得到数据huffmanCode

    void encode();  //压缩编码,由huffmanCode得到Strcode

    void decode();  //解压缩，由huffmanCode和Strcode得到decodeStr

    void countRate();  //计算压缩率

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

/*string Solution::readFlie(string filePath)
void Solution::readFlie(string filePath)
{
    ifstream infile;
    infile.open(filePath.data());
    assert(infile.is_open());
    char c;
    infile >> noskipws;
    while (!infile.eof())
    {
        infile >> c;
        //cout<<c<<endl;
        originalStr.push_back(c);
    }
    infile.close();
    return ans;
}*/

void Solution::statistics(string &txt) {
    // len = txt.length();
    int count[128];
    double mat[128][128];
    memset(count, 0, sizeof(count));
    memset(mat, 0, sizeof(mat));

    count[(int)txt[0]]++;
    for (int i = 1; i < len; i++) {
        count[(int)txt[i]]++;
        mat[(int)txt[i - 1]][(int)txt[i]]++;
    }

    for (int i = 0; i < 128; i++) {
        int sum1 = 0, sum2 = 0;
        for (int j = 0; j < 128; j++) {
            sum1 += mat[i][j];
            sum2 += mat[j][i];
        }
        for (int k = 0; k < 128; k++) {
            if (sum1 != 0) {
                p1.val[i][k] = mat[i][k] / sum1;
                // if(mat[i][k] != 0)
                // cout<<"i:"<<i<<" k:"<<k<<" val:"<<p1.val[i][k]<<"
                // mat:"<<mat[i][k]<<" sum1:"<<sum1<<endl;
            }
            if (sum2 != 0) {
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

void Solution::writeBinaryFile(string huffcode, string fileToWrite) {
    ofstream outFile(fileToWrite.c_str(),
                     ios::binary | ios::out);  //以二进制写模式打开文件
    if (!outFile) {
        cout << "New file open error." << endl;
        return;
    }
    int wlen = 8;
    int i;
    for (i = 0; i < huffcode.length(); i = i + wlen) {
        char *p = (char *)malloc(wlen * sizeof(char));
        string sub = huffcode.substr(i, i + wlen);
        bitset<8> bit(sub);
        char a = bit.to_ulong();  //这里为0-256
        outFile.put(a);
        free(p);
    }
    outFile.close();
}

void Solution::calculatetemp1() {
    for (int i = 0; i < 128; i++) {
        double cnt = 0;
        double m = MAX;
        for (int j = 0; j < 128; j++) {
            cnt += p1.val[i][j];
            if (p1.val[i][j] > 0) m = min(m, p1.val[i][j]);
            if (cnt >= yuzhi) {
                break;
            }
        }
        if (m == MAX) m = 0;
        forwardtemp1[i] = m;
    }
    for (int i = 0; i < 128; i++) {
        double cnt = 0;
        double m = MAX;
        for (int j = 0; j < 128; j++) {
            cnt += n1.val[i][j];
            if (n1.val[i][j] > 0) m = min(m, n1.val[i][j]);
            if (cnt >= yuzhi) {
                break;
            }
        }
        if (m == MAX) m = 0;
        backtemp1[i] = m;
    }
}

void Solution::calculatetempn() {
    for (int i = 0; i < originalStr.size(); i++) {
        int k = originalStr[i];
        double ans = forwardtemp1[k];
        int j = i + 1;
        for (; j < 128 && j - i <= 3; j++) {
            forwardtempn[i][j - i] = ans;
            k = originalStr[j];
            ans *= forwardtemp1[k];
        }
    }
    for (int i = 0; i < originalStr.size(); i++) {
        int k = originalStr[i];
        double ans = backtemp1[k];
        int j = i + 1;
        for (; j < 128 && j - i <= 3; j++) {
            backtempn[i][j - i] = ans;
            k = originalStr[j];
            ans *= backtemp1[k];
        }
    }
}

void Solution::judgeRelation() {
    int posi = 0;
    int step;
    int flag = 0;
    calculatetemp1();
    calculatetempn();
    while (posi < len) {
        step = 1;
        for (int i = posi; i < MAXWINDOW + posi; i++) {
            if (p1.val[i][i + step] >= forwardtemp1[i] &&
                n1.val[i][i + step] >= backtemp1[i]) {
                flag = i + step;
                continue;
            } else {
                flag = i;
                break;
            }
        }

        //一阶已经完成
        step = posi + 2;
        // flag是滑动窗口的右边界
        if (flag - posi > 1)  //零阶直接跳出
        {
            int l = flag;
            for (int j = step; j <= l; j++) {
                if (j - posi == 2)
                    if (p2.val[posi][j] >= forwardtempn[posi][j - posi] &&
                        n2.val[posi][j] >= backtempn[posi][j - posi]) {
                        flag = j;
                        continue;
                    } else {
                        flag = j - 1;
                        break;
                    }
                if (j - posi == 3) {
                    if (p3.val[posi][j] >= forwardtempn[posi][j - posi] &&
                        n3.val[posi][j] >= backtempn[posi][j - posi]) {
                        flag = j;
                        continue;
                    } else {
                        flag = j - 1;
                        break;
                    }
                }
            }
        }
        string s = originalStr.substr(posi, flag + 1 - posi);

        pair<map<string, int>::iterator, bool> Insert_Pair;

        Insert_Pair = stringset.insert(map<string, int>::value_type(s, 1));

        if (!Insert_Pair.second) {
            stringset[s]++;
        }
        posi += s.size();
    }
}
void Solution::countRate() {
    compressionRate = Strcode.size() / (originalStr.size() * 8);
    cout << "The compression rate is " << compressionRate << endl;
}
void Solution::buildTree() {
    //由stringset构建哈夫曼树，然后调用getHuffmanCode()
    vector<node> Vec(stringset.begin(), stringset.end());
    int KeyNumber = Vec.size();
    priority_queue<TreeNode *, vector<TreeNode *>, compare> Queue;
    for (int i = 0; i < KeyNumber; i++) {
        TreeNode *tempnode = new TreeNode();
        tempnode->left = nullptr;
        tempnode->right = nullptr;
        tempnode->Node = Vec.back();
        Vec.pop_back();
        Queue.push(tempnode);
    }  //以上为将stringset中的内容转化为哈夫曼树的节点并存入优先队列

    TreeNode *node1 = new TreeNode();
    TreeNode *node2 = new TreeNode();

    for (int i = 0; i < KeyNumber - 1; i++) {
        node1 = Queue.top();
        Queue.pop();

        node2 = Queue.top();
        Queue.pop();

        TreeNode *Newnode = new TreeNode();
        Newnode->left = node1;
        Newnode->right = node2;
        Newnode->Node.second = node1->Node.second + node2->Node.second;

        Queue.push(Newnode);
    }  //构造哈夫曼树，得到根节点root
    TreeNode *root = Queue.top();
    getHuffmanCode(root, "");
    delete node1;
    delete node2;
}

void Solution::encode() {   //如果字符字串有/t开头的可能会出问题？
    int TempLength = Longest;
    //设置一个TempLength变量，初始为最长长度，每次先检测当前字符串头的前TempLength个字符
    //之和，看看是不是map中对应的一个单词
    while (Strcode.length() > Longest) {
        auto iter = huffmanCode.find(Strcode.substr(0, TempLength));
        if (iter != huffmanCode.end()) {
            StrOf01 += iter->second;
            Strcode.erase(Strcode.begin(), Strcode.begin() + TempLength);
            TempLength = Longest;
        }
        TempLength--;
    }
    string s;  //储存map部分
    for (auto iter = CodeToWord.begin(); iter != CodeToWord.end(); iter++) {
        s += "\a";
        s += iter->first;
        s += "\a";
        s += iter->second;
    }
    s += "\a";
    s += "\t";
    Str_File_Connected = StrOf01 + s;
}

void Solution::decode() {
    //得到StrOf01部分
    int temp = 0;
    while (Str_File_Connected.at(temp) != '\a') temp++;
    string StrOf01 = Str_File_Connected.substr(0, temp);
    Str_File_Connected.erase(Str_File_Connected.begin(),
                             Str_File_Connected.begin() + temp + 1);

    //载入map部分
    CodeToWord.clear();
    temp = 0;
    int tempbegin = 0;
    while (Str_File_Connected.at(temp) != '\t') {
        while (Str_File_Connected.at(temp) != '\a') {
            temp++;
        }
        // cout << "test" << endl;
        string first = Str_File_Connected.substr(tempbegin, temp - tempbegin);
        temp++;
        tempbegin = temp;
        while (Str_File_Connected.at(temp) != '\a') {
            temp++;
        }
        // cout << "test2" << endl;
        string second = Str_File_Connected.substr(tempbegin, temp - tempbegin);
        temp++;
        tempbegin = temp;
        CodeToWord.insert(make_pair(first, second));
    }

    //翻译部分
    int TempLength = 0;
    Str_File_Connected.clear();
    while (!StrOf01.empty()) {
        auto iter = CodeToWord.find(StrOf01.substr(0, TempLength));
        if (iter != CodeToWord.end()) {
            Str_File_Connected += iter->second;
            StrOf01.erase(StrOf01.begin(), StrOf01.begin() + TempLength);
            TempLength = 0;
        }
        TempLength++;
    }
}

int main(int argc, char *argv[]) {
    Solution s;
    s.readFlie(argv[1]);
    s.statistics(s.originalStr);
    s.calculatetempn();
    s.judgeRelation();
    s.buildTree();
    s.encode();
    s.writeBinaryFile(s.Strcode, argv[2]);
    s.countRate();
}
