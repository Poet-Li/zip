#define MAX 3001
#define MAXWINDOW 3
#define yuzhi 0.5
#define Longest 4  //最长的字符字串的长度
#include <algorithm>
#include <bitset>
#include <cassert>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <vector>
using namespace std;
typedef pair<string, int> node;

class Matrix {
   public:
    double val[128][128];
    Matrix() { memset(val, 0, sizeof(val)); }
    Matrix operator*(const Matrix &b) {
        Matrix res;
        for (int i = 0; i < 128; i++)
            for (int j = 0; j < 128; j++)
                for (int k = 0; k < 128; k++) {
                    res.val[i][j] = (res.val[i][j] + val[i][k] * b.val[k][j]);
                }
        return res;
    }
    void operator=(const Matrix &b) {
        for (int i = 0; i < 128; i++)
            for (int j = 0; j < 128; j++) val[i][j] = b.val[i][j];
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
    //用时间换空间！！！

    Matrix p1;  //一阶向前转移矩阵
    Matrix n1;  //一阶向后转移矩阵
    Matrix p2;  //二阶向前转移矩阵
    Matrix n2;  //二阶向后转移矩阵
    Matrix p3;  //三阶向前转移矩阵
    Matrix n3;  //三阶向后转移矩阵

    map<string, int> stringset;       //字符子串与权重的一一映射
    map<string, string> huffmanCode;  //字符子串与编码的一一映射
    map<string, string> CodeToWord;   //编码到字符字串的映射
    // string originalStr;                  //整个字符串对应的编码集
    string StrOf01;    //用字符串存的01串嗷
    string huffTable;  //映射表

    string Str_File_Connected;  //治达用这个string来做文件操作就行
    // func

    //将txt文件读入originalStr中
    void readFlie(string filePath);

    //数据统计模块，用originalStr计算出一二三阶的向前向后转移阈值与转移矩阵（共6个值，6个矩阵需要计算）
    void statistics(string &txt);

    void calculatetemp1();  //计算阈值

    double getforwardtempn(int str, int sq);

    double getbackwardtempn(int str, int sq);

    void writeBinaryFile(string filePath);  // 把字符串写进txt文件中

    void judgeRelation();

    //调用下面那个buildTree函数就行，这个是buildTree中要用到的一个递归函数
    void getHuffmanCode(TreeNode *treenode, string code);

    void buildTree();  //由stringset构建哈夫曼树，得到数据huffmanCode

    void encode();  //压缩编码,由huffmanCode得到originalStr

    void decode();  //解压缩，由huffmanCode和originalStr得到decodeStr

    void countRate();  //计算压缩率
};

void Solution::readFlie(string filePath) {
    ifstream infile;
    infile.open(filePath.data());
    assert(infile.is_open());
    char c;
    infile >> noskipws;
    while (!infile.eof()) {
        infile >> c;
        // cout<<c<<endl;
        originalStr.push_back(c);
    }
    infile.close();
}

void Solution::statistics(string &txt) {
    len = txt.length();
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
                p1.val[i][k] = (mat[i][k] / sum1);
                // if(mat[i][k] != 0)
                // cout<<"i:"<<i<<" k:"<<k<<" val:"<<p1.val[i][k]<<"
                // mat:"<<mat[i][k]<<" sum1:"<<sum1<<endl;
            }
            if (sum2 != 0) {
                n1.val[i][k] = (mat[i][k] / sum2);
            }
        }
    }
    p2 = p1 * p1;
    p3 = p2 * p1;
    n2 = n1 * n1;
    n3 = n2 * n1;
    return;
}

void Solution::writeBinaryFile(string fileToWrite) {
    ofstream outFile(fileToWrite.c_str(),
                     ios::binary | ios::out);  //以二进制写模式打开文件
    if (!outFile) {
        cout << "New file open error." << endl;
        return;
    }

    //得到StrOf01部分
    int temp = 0;
    while (Str_File_Connected.at(temp) != '\a') temp++;
    string huffcode = Str_File_Connected.substr(0, temp);
    string hufftable =
        Str_File_Connected.substr(temp + 1, Str_File_Connected.length() - temp);

    int wlen = 8;
    int i;
    for (i = 0; i < huffcode.length(); i = i + wlen) {
        string sub = huffcode.substr(i, wlen);
        bitset<8> bit(sub);
        char a = bit.to_ulong();  //这里为0-256
        outFile.put(a);
    }
    i -= wlen;
    string sub = huffcode.substr(i + 1, huffcode.length() - i);
    bitset<8> bit(sub);
    char a = bit.to_ulong();  //这里为0-256
    outFile.put(a);
    for (i = 0; i < hufftable.length(); i++) {
        outFile.put(hufftable[i]);
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

inline double Solution::getforwardtempn(int str, int sq) {
    int c = originalStr[str];
    double ans = forwardtemp1[c];
    for (int i = str + 1; i <= str + sq; i++) {
        c = originalStr[i];
        ans *= forwardtemp1[c];
    }
    return ans;
}

inline double Solution::getbackwardtempn(int str, int sq) {
    int c = originalStr[str];
    double ans = backtemp1[c];
    for (int i = str + 1; i <= str + sq; i++) {
        c = originalStr[i];
        ans *= backtemp1[c];
    }
    return ans;
}

void Solution::judgeRelation() {
    int posi = 0;
    int step;
    int flag = 0;
    calculatetemp1();
    // for (int i = 0; i < 128; i++) {
    //     char k = i;
    //     string s(1, k);
    //     stringset.insert(map<string, int>::value_type(s, 0));
    // }
    while (posi < len) {
        step = 1;
        for (int i = posi; i < MAXWINDOW + posi; i++) {
            int p = originalStr[i];
            int w = originalStr[i + step];
            if (p1.val[p][w] > forwardtemp1[p] && n1.val[p][w] > backtemp1[p]) {
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
                if (j - posi == 2) {
                    int p = originalStr[posi];
                    int b = originalStr[posi + 2];
                    if (p2.val[p][b] > getforwardtempn(posi, 2) &&
                        n2.val[p][b] > getbackwardtempn(posi, 2)) {
                        flag = j;
                        continue;
                    } else {
                        flag = j - 1;
                        break;
                    }
                } else if (j - posi == 3) {
                    step = posi + 3;
                    {
                        int p = originalStr[posi];
                        int b = originalStr[posi + 3];
                        if (p3.val[p][b] > getforwardtempn(posi, 3) &&
                            n3.val[p][b] > getbackwardtempn(posi, 3)) {
                            flag = j;
                            continue;
                        } else {
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

        if (!Insert_Pair.second) {
            stringset[s]++;
        }
        posi += s.size();
        // cout << posi << endl;
    }
    // cout << cnt << endl;
}
void Solution::countRate() {
    double a = StrOf01.size() + huffTable.size();
    double b = len;
    compressionRate = a / len / 8;
    cout << a << endl << b << endl;
    cout << "The compression rate is " << compressionRate << endl;
}

void Solution::getHuffmanCode(TreeNode *treenode, string code) {
    treenode->huffcode = code;
    if (treenode->left != NULL) getHuffmanCode(treenode->left, code + "0");
    if (treenode->right != NULL) getHuffmanCode(treenode->right, code + "1");
    if ((treenode->right == NULL) && (treenode->left == NULL)) {
        huffmanCode[treenode->Node.first] = treenode->huffcode;
        CodeToWord[treenode->huffcode] = treenode->Node.first;
    }
}

void Solution::buildTree() {
    //由stringset构建哈夫曼树，然后调用getHuffmanCode()
    vector<node> Vec(stringset.begin(), stringset.end());
    int KeyNumber = Vec.size();
    priority_queue<TreeNode *, vector<TreeNode *>, compare> Queue;
    for (int i = 0; i < KeyNumber; i++) {
        TreeNode *tempnode = new TreeNode();
        tempnode->left = NULL;
        tempnode->right = NULL;
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

void Solution::encode() {  //如果字符字串有/t开头的可能会出问题？
    int TempLength = Longest;
    //设置一个TempLength变量，初始为最长长度，每次先检测当前字符串头的前TempLength个字符
    //之和，看看是不是map中对应的一个单词
    while (originalStr.length() > Longest) {
        map<string, string>::iterator iter =
            huffmanCode.find(originalStr.substr(0, TempLength));
        if (iter != huffmanCode.end()) {
            StrOf01 += iter->second;
            originalStr.erase(originalStr.begin(),
                              originalStr.begin() + TempLength);
            TempLength = Longest;
            continue;
        }
        TempLength--;
        if (TempLength == 0) {
            cout << "fail" << endl;
        }
    }
    // cout << "test" << endl;
    // string huffTable; //储存map部分
    for (map<string, int>::iterator iter = stringset.begin();
         iter != stringset.end(); iter++) {
        huffTable += "\a";
        huffTable += iter->first;
        huffTable += "\a";
        huffTable += to_string(iter->second);
    }
    huffTable += "\a";
    huffTable += "\t";
    Str_File_Connected = StrOf01 + huffTable;
}

void Solution::decode() {
    //得到StrOf01部分
    int temp = 0;
    while (Str_File_Connected.at(temp) != '\a') temp++;
    string StrOf01 = Str_File_Connected.substr(0, temp);
    Str_File_Connected.erase(Str_File_Connected.begin(),
                             Str_File_Connected.begin() + temp + 1);

    //载入map部分
    stringset.clear();
    temp = 0;
    int tempbegin = 0;
    while (Str_File_Connected.at(temp) != '\t') {
        while (Str_File_Connected.at(temp) != '\a') {
            temp++;
        }
        string first = Str_File_Connected.substr(tempbegin, temp - tempbegin);
        temp++;
        tempbegin = temp;
        while (Str_File_Connected.at(temp) != '\a') {
            temp++;
        }
        int second = stoi(Str_File_Connected.substr(tempbegin, temp - tempbegin));
        temp++;
        tempbegin = temp;
        stringset.insert(make_pair(first, second));
    }

    //通过载入的stringset得到CodeToWord
    buildTree();

    //翻译部分
    int TempLength = 0;
    Str_File_Connected.clear();
    while (!StrOf01.empty()) {
        map<string, string>::iterator iter =
            CodeToWord.find(StrOf01.substr(0, TempLength));
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
    // s.readFlie(argv[2]);
    s.readFlie("txt04.txt");
    cout << 1 << endl;
    s.statistics(s.originalStr);
    // for(int i = 0; i < 128; i++)
    // {
    // 	for(int j = 0; j < 128; j++)
    // 	{
    // 		if(s.p1.val[i][j] != 0)	cout<<"i: "<<i<<" j: "<<j<<" val:
    // "<<s.p1.val[i][j]<<endl; 		if(s.n2.val[i][j] != 0)
    // cout<<"i:
    // "<<i<<" j:
    // "<<j<<" val: "<<s.n2.val[i][j]<<endl;
    // 	}
    // }
    cout << 2 << endl;
    s.judgeRelation();
    cout << 3 << endl;
    s.buildTree();
    cout << 4 << endl;
    s.encode();
    cout << 5 << endl;
    s.writeBinaryFile("out.txt");
    cout << 6 << endl;
    s.countRate();
}
