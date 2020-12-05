/*
实现buildTree函数
杰哥写的模板里面函数都是没有参数的，我实现就按照没有参数的写了。
实际上调用的参数是三个全局的map变量
*/

#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <vector>

using namespace std;

typedef pair<string, int> node;

map<string, int> stringset;       //字符子串与权重的一一映射
map<string, string> huffmanCode;  //字符子串与编码的一一映射
map<string, string> CodeToWord;   //编码到单词的映射

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
void getHuffmanCode(TreeNode* treenode, string code) {
    treenode->huffcode = code;
    if (treenode->left != nullptr) getHuffmanCode(treenode->left, code + "0");
    if (treenode->right != nullptr) getHuffmanCode(treenode->right, code + "1");
    if ((treenode->right == nullptr) && (treenode->left == nullptr)) {
        huffmanCode[treenode->Node.first] = treenode->huffcode;
        CodeToWord[treenode->huffcode] = treenode->Node.first;
    }
}

void buildTree() {  //由stringset构建哈夫曼树，然后调用getHuffmanCode()
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
    getHuffmanCode(root, "");
    delete node1;
    delete node2;
}


// 以下为本地单文件测试代码
int main() {
    stringset["One  "] = 1;
    stringset["Two "] = 2;
    stringset["Forty-Four  "] = 44;
    stringset["NINE    "] = 9;
    stringset["99\n    "] = 99;
    stringset["a thousand minor one"] = 999;
    buildTree();
    cout << endl << "----------" << endl;
    cout << huffmanCode.at("One  ") << endl;
    cout << huffmanCode.at("Two ") << endl;
    cout << huffmanCode.at("Forty-Four  ") << endl;
    cout << huffmanCode.at("NINE    ") << endl;
    cout << huffmanCode.at("99\n    ") << endl;
    cout << huffmanCode.at("a thousand minor one") << endl;

    return 0;
}