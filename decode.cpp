/*
通过encode()函数即可完成解码
encode()的三个参数分别为：

1.string类型 压缩文件名(二进制文件)   
如："Huffman.bin"  (要加文件名后缀)    
若文件不存在，则会报错并退出函数。

2.string类型 解压后文件名             
如："out.txt" (也要加文件名后缀)      
若文件不存在，则会创建并且写入。若文件已经存在，则会在其原本内容后面加上。

3.map<string, string> CodeToWord;  //编码到单词的映射

*/

#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <vector>

#define MaxLengthOfBin 99999999  //二进制文件的最长长度

using namespace std;

void encode(string InFileName, string OutFileName,
            map<string, string> CodeToWord) {
    //如果传入的两个文件名不包括后缀的话，可以统一到这里加上
    // InFileName += ".bat";   //或.bin
    // OutFileName += ".txt";

    char temp[MaxLengthOfBin];
    //将二进制文件的01串存入程序的string类型中，需要通过char*中转

    ifstream InFile(InFileName);
    ofstream OutFile(OutFileName);
    if (!InFile.is_open()) {
        cout << "Open File Error!" << endl;
        return;
    }   //若打开失败则退出函数

    InFile.read(temp, MaxLengthOfBin);
    string Strcode = temp;
    string decodeStr;   //用于记录解码后的内容
    int TempLength = 0;
    while (!Strcode.empty()) {
        auto iter = CodeToWord.find(Strcode.substr(0, TempLength));
        if (iter != CodeToWord.end()) {
            decodeStr += iter->second;
            Strcode.erase(Strcode.begin(), Strcode.begin() + TempLength);
            TempLength = 0;
        }
        TempLength++;
    }
    OutFile << decodeStr;   //将解码得到的内容输出到文件中
}