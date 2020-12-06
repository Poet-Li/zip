#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <vector>

using namespace std;

void decode(string StrOf01, string &StrUnzip, map<string, string> CodeToWord) {
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