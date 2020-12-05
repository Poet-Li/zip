/*

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
    TreeNode(node a) {
        Node = a;
        right = nullptr;
        left = nullptr;
    }
    TreeNode() {
        right = nullptr;
        left = nullptr;
    }
};

struct compare {  //用于优先队列的比较
    bool operator()(TreeNode*& a, TreeNode*& b) {
        return a->Node.second > b->Node.second;
    }
};

void getHuffmanCode(TreeNode* treenode, string code) {
    treenode->huffcode = code;
    if (treenode->left != nullptr) getHuffmanCode(treenode->left, code + "0");
    if (treenode->right != nullptr) getHuffmanCode(treenode->right, code + "1");
    if ((treenode->right == nullptr) && (treenode->left == nullptr)) {
        huffmanCode[treenode->Node.first] = treenode->huffcode;
        CodeToWord[treenode->huffcode] = treenode->Node.first;
    }
}

void buildTree() {  //由stringset构建哈夫曼树，得到数据huffmanCode
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
    }

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
    }
    TreeNode* root = Queue.top();
    getHuffmanCode(root, "");
}




//以下为测试代码
// int main() {
//     stringset["aaa"] = 1;
//     stringset["bbb"] = 2;
//     stringset["DD"] = 44;
//     stringset["EE"] = 9;
//     stringset["f"] = 99;
//     stringset["ggg"] = 999;
//     buildTree();
//     cout << endl << "----------" << endl;
//     cout << huffmanCode.at("aaa") << endl;
//     cout << huffmanCode.at("bbb") << endl;
//     cout << huffmanCode.at("DD") << endl;
//     cout << huffmanCode.at("EE") << endl;
//     cout << huffmanCode.at("f") << endl;
//     cout << huffmanCode.at("ggg") << endl;
//     cout << CodeToWord.at("01") << endl;
//     // cout << CodeToWord.end() << endl;
//     return 0;
// }