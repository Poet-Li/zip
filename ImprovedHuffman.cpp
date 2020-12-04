#include <iostream>
#include <vector>
#include <string>
#include <map>
using namespace std;
class matrix
{
	public:
		int size;
		double** val;
		matrix(int size)
		{
			size = size;
			val = (double**) malloc(size * size * sizeof(double));
		}
};
class Solution
{
public:
    //data
    string originalStr; //初始字符集
    int len;            //字符集长度

    double forwardtemp1; //一阶向前转移概率阈值
    double backtemp1;    //一阶向后转移概率阈值
    double forwardtemp2; //二阶向前转移概率阈值
    double backtemp2;    //二阶向后转移概率阈值
    double forwardtemp3; //三阶向前转移概率阈值
    double backtemp3;    //三阶向后转移概率阈值

    matrix p1; //一阶向前转移矩阵
    matrix n1; //一阶向后转移矩阵
    matrix p2; //二阶向前转移矩阵
    matrix n2; //二阶向后转移矩阵
    matrix p3; //三阶向前转移矩阵
    matrix n3; //三阶向后转移矩阵

    map<string, int> stringset;      //字符子串与权重的一一映射
    map<string, string> huffmanCode; //字符子串与编码的一一映射
    string Strcode;                  //整个字符串对应的编码集
    string decodeStr;                //解码后的字符串
    //func
    void readFlie()
    {

    } //将txt文件读入originalStr中

    void statistics()
    {

    } //数据统计模块，用originalStr计算出一二三阶的向前向后转移阈值与转移矩阵（共6个值，6个矩阵需要计算）

    void judgeRelation()
    {

    } //相关性判断模块，由originalStr和转移矩阵构建stringset

    void bulidTree()
    {

    } //由stringset构建哈夫曼树，得到数据huffmanCode

    void encode()
    {

    } //压缩编码,由huffmanCode得到Strcode

    void decode()
    {

    }//解压缩，由huffmanCode和Strcode得到decodeStr

    void countRate(){
        
    }//计算压缩率
    void solve()
    {
        readFlie();
        statistics();
        judgeRelation();
        bulidTree();
        encode();
        decode();
        countRate();
    }
};
int main()
{
    Solution s;
    s.solve();
}