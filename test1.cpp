#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <vector>
#define Longest 30
using namespace std;

typedef pair<string, int> node;

//三个map不用作全局变量
// map<string, int> stringset;       //字符子串与权重的一一映射
// map<string, string> huffmanCode;  //字符子串与编码的一一映射
// map<string, string> CodeToWord;   //编码到单词的映射

class TreeNode {
   public:
    node Node;
    TreeNode* left;
    TreeNode* right;
    string huffcode;
};

struct compare {  //用于优先队列的比较
    bool operator()(TreeNode*& a, TreeNode*& b) {
        return a->Node.second > b->Node.second;
    }
};

//通过构造好的哈夫曼树获得哈夫曼编码，存入两个map中；
void getHuffmanCode(TreeNode* treenode, string code,
                    map<string, string>& huffmanCode,
                    map<string, string>& CodeToWord) {
    treenode->huffcode = code;
    if (treenode->left != nullptr)
        getHuffmanCode(treenode->left, code + "0", huffmanCode, CodeToWord);
    if (treenode->right != nullptr)
        getHuffmanCode(treenode->right, code + "1", huffmanCode, CodeToWord);
    if ((treenode->right == nullptr) && (treenode->left == nullptr)) {
        huffmanCode[treenode->Node.first] = treenode->huffcode;
        CodeToWord[treenode->huffcode] = treenode->Node.first;
    }
}

void buildTree(map<string, int> stringset, map<string, string>& huffmanCode,
               map<string, string>& CodeToWord) {
    //由stringset构建哈夫曼树，然后调用getHuffmanCode()
    vector<node> Vec(stringset.begin(), stringset.end());
    int KeyNumber = Vec.size();
    priority_queue<TreeNode*, vector<TreeNode*>, compare> Queue;
    for (int i = 0; i < KeyNumber; i++) {
        TreeNode* tempnode = new TreeNode();
        tempnode->left = nullptr;
        tempnode->right = nullptr;
        tempnode->Node = Vec.back();
        Vec.pop_back();
        Queue.push(tempnode);
    }  //以上为将stringset中的内容转化为哈夫曼树的节点并存入优先队列

    TreeNode* node1 = new TreeNode();
    TreeNode* node2 = new TreeNode();

    for (int i = 0; i < KeyNumber - 1; i++) {
        node1 = Queue.top();
        Queue.pop();

        node2 = Queue.top();
        Queue.pop();

        TreeNode* Newnode = new TreeNode();
        Newnode->left = node1;
        Newnode->right = node2;
        Newnode->Node.second = node1->Node.second + node2->Node.second;

        Queue.push(Newnode);
    }  //构造哈夫曼树，得到根节点root
    TreeNode* root = Queue.top();
    getHuffmanCode(root, "", huffmanCode, CodeToWord);
    delete node1;
    delete node2;
}

void encode(string StrOG, string& StrOf01, map<string, string> huffmanCode) {
    int TempLength = Longest;
    //设置一个TempLength变量，初始为最长长度，每次先检测当前字符串头的前TempLength个字符
    //之和，看看是不是map中对应的一个单词
    while (StrOG.length() > Longest) {
        auto iter = huffmanCode.find(StrOG.substr(0, TempLength));
        if (iter != huffmanCode.end()) {
            StrOf01 += iter->second;
            StrOG.erase(StrOG.begin(), StrOG.begin() + TempLength);
            TempLength = Longest;
            continue;
        }
        TempLength--;
    }
}

void decode(string StrOf01, string& StrUnzip, map<string, string> CodeToWord) {
    int TempLength = 0;
    while (!StrOf01.empty()) {
        auto iter = CodeToWord.find(StrOf01.substr(0, TempLength));
        if (iter != CodeToWord.end()) {
            StrUnzip += iter->second;
            StrOf01.erase(StrOf01.begin(), StrOf01.begin() + TempLength);
            TempLength = 0;
        }
        TempLength++;
    }
}

// 以下为本地单文件测试代码
int main() {
    map<string, int> stringset;       //字符子串与权重的一一映射
    map<string, string> huffmanCode;  //字符子串与编码的一一映射
    map<string, string> CodeToWord;   //编码到单词的映射
    stringset["One  "] = 1;
    stringset["Two "] = 2;
    stringset["Forty-Four  "] = 44;
    stringset["NINE    "] = 9;
    stringset["99\n    "] = 99;
    stringset["a thousand minor one|||||"] = 999;
    buildTree(stringset, huffmanCode, CodeToWord);
    cout << endl << "----------" << endl;
    cout << huffmanCode.at("One  ") << endl;
    cout << huffmanCode.at("Two ") << endl;
    cout << huffmanCode.at("Forty-Four  ") << endl;
    cout << huffmanCode.at("NINE    ") << endl;
    cout << huffmanCode.at("99\n    ") << endl;
    cout << huffmanCode.at("a thousand minor one|||||") << endl;
    string StrOG =
        "One  Two a thousand minor one|||||a thousand minor one|||||a thousand "
        "minor one|||||a thousand minor one|||||a thousand minor one|||||a "
        "thousand minor one|||||";
    // string StrOf01 = "0000000001111111";
    string StrOf01 = "";
    string StrUnzip = "";
    encode(StrOG, StrOf01, huffmanCode);
    cout << "-----StrOf01-----" << endl << StrOf01 << endl << endl;
    decode(StrOf01, StrUnzip, CodeToWord);
    cout << "-----StrUnzip-----" << endl << StrUnzip << endl << endl;
    return 0;
}
