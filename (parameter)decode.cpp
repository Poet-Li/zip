/*
实现decode函数
传入的是读二进制文件然后写入程序的string中的010100100011101010和buildtree中得到的另一个map
返回的是解压缩后的全文，存在string里面
*/


#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <vector>

using namespace std;

string decode(string StrOf01, map<string, string> CodeToWord) {
    string StrUnzip;
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
    return StrUnzip;
}