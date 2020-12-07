/*
实现encode函数
注意#define了东西
传入的是原始的已经读入了程序中string的压缩前的全文，和buildtree构造的map之一
返回的是压缩得到的存在string里面的0101011101010100101........
*/


#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <vector>

#define Longest 4

using namespace std;

string encode(string StrOG, map<string, string> huffmanCode) {
    string StrOf01;
    int TempLength = Longest;
    //设置一个TempLength变量，初始为最长长度，每次先检测当前字符串头的前TempLength个字符
    //之和，看看是不是map中对应的一个单词
    while (StrOG.length() > Longest) {
        auto iter = huffmanCode.find(StrOG.substr(0, TempLength));
        if (iter != huffmanCode.end()) {
            StrOf01 += iter->second;
            StrOG.erase(StrOG.begin(), StrOG.begin() + TempLength);
            TempLength = Longest;
        }
        TempLength--;
    }
    return StrOf01;
}