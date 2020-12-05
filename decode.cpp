#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <vector>

using namespace std;

map<string, string> CodeToWord;  //编码到单词的映射

string Strcode;
string decodeStr;

void encode() {
    string Coding = Strcode;
    int TempLength = 0;
    while (!Coding.empty()) {
        auto iter = CodeToWord.find(Coding.substr(0, TempLength));
        if (iter != CodeToWord.end()) {
            decodeStr += iter->second;
            Coding.erase(Coding.begin(), Coding.begin() + TempLength);
            TempLength = 0;
        }
        TempLength++;
    }
}