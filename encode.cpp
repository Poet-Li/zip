#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <vector>

#define Longest 4

using namespace std;

void encode(string StrOG, string &StrOf01, map<string, string> huffmanCode) {
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
}