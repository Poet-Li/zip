#include<bits/stdc++.h>
using namespace std;
class Matrix
{
	public:
		double val[128][128];
		Matrix()
		{
			memset(val, 0, sizeof(val));
		}
		Matrix operator*(const Matrix b)
		{
			Matrix res;
			for (int i = 0; i < 128; i++)
				for (int j = 0; j < 128; j++)
					for (int u = 0; u < 128; u++)
						res.val[i][j] = (res.val[i][j] + val[i][u]*b.val[u][j]);
			return res;
    	}
		Matrix operator=(const Matrix b)
		{
			Matrix res;
			for (int i = 0; i < 128; i++)
				for (int j = 0; j < 128; j++)
					res.val[i][j] = val[i][j];
			return res;
		}

};
class Solution
{
public:
    //data
    string originalStr; //初始字符集
    int len;            //字符集长度
    double compressionRate;

    double forwardtemp[MAX]; //一阶向前转移概率阈值
    double backtemp[MAX];    //一阶向后转移概率阈值

    Matrix p1; //一阶向前转移矩阵
    Matrix n1; //一阶向后转移矩阵
    Matrix p2; //二阶向前转移矩阵
    Matrix n2; //二阶向后转移矩阵
    Matrix p3; //三阶向前转移矩阵
    Matrix n3; //三阶向后转移矩阵

    map<string, int> stringset;      //字符子串与权重的一一映射
    map<string, string> huffmanCode; //字符子串与编码的一一映射
    string Strcode;                  //整个字符串对应的编码集
    string decodeStr;                //解码后的字符串
    //func

    //将txt文件读入originalStr中
    string readFlie(string filePath);
     
     //数据统计模块，用originalStr计算出一二三阶的向前向后转移阈值与转移矩阵（共6个值，6个矩阵需要计算）
    void statistics(string txt);

    void writeBinaryFile(string huffcode, string fileToWrite);
  

    void buildTree(); //由stringset构建哈夫曼树，得到数据huffmanCode

    void encode(); //压缩编码,由huffmanCode得到Strcode

    void decode(); //解压缩，由huffmanCode和Strcode得到decodeStr

    void countRate(){
        
    }//计算压缩率

    // 我个人不建议用这个 不如一个一个函数调用
    /*void solve()
    void countRate(); //计算压缩率
    void solve()
    {
        readFlie();
        statistics();
        judgeRelation();
        buildTree();
        encode();
        decode();
        countRate();
    }*/
};

string Solution::readFlie(string filePath)
{
    string ans;
    ifstream infile;
    infile.open(filePath.data());
    assert(infile.is_open());
    char c;
    infile >> noskipws;
    while (!infile.eof())
    {
        infile>>c;
        //cout<<c<<endl;
        ans.push_back(c);
    }
    infile.close();
    return ans;
}

void Solution::statistics(string txt)
{
	int len = txt.length();
	int count[128];
	double mat[128][128];
	memset(count, 0, sizeof(count));
	memset(mat, 0, sizeof(mat));

	count[(int)txt[0]]++;
	for(int i = 1; i < len; i++)
	{
		count[(int)txt[i]]++;
		mat[(int)txt[i-1]][(int)txt[i]]++;
	}

	for(int i = 0; i < 128; i++)
	{
		int sum1 = 0, sum2 = 0;
		for(int j = 0; j < 128; j++)
		{
			sum1 += mat[i][j];
			sum2 += mat[j][i];
		}
		for(int k = 0; k < 128; k++)
		{
			if(sum1 != 0)
			{
				p1.val[i][k] = mat[i][k] / sum1;
				//if(mat[i][k] != 0)
					//cout<<"i:"<<i<<" k:"<<k<<" val:"<<p1.val[i][k]<<" mat:"<<mat[i][k]<<" sum1:"<<sum1<<endl;
			}
			if(sum2 != 0)
			{
				n1.val[i][k] = mat[i][k] / sum2;
			}
		}
	}
    p2 = p1 * p1;
    p3 = p2 * p1;
    n2 = n1 * n1;
    n3 = n2 * n1;

	return ;
}

void Solution::writeBinaryFile(string huffcode, string fileToWrite)
{
    ofstream outFile(fileToWrite.c_str(), ios::binary | ios::out);  //以二进制写模式打开文件
    if (!outFile) {
        cout << "New file open error." << endl;
        return ;
    }
    int wlen = 8;
    int i;
    for(i = 0; i < huffcode.length(); i = i + wlen)
    {
        char* p = (char*)malloc(wlen*sizeof(char));
        string sub = huffcode.substr(i, i + wlen);
        bitset<8> bit(sub);
        char a = bit.to_ulong(); //这里为0-256
        outFile.put(a);
        free(p);
    }
    outFile.close();
}

int main(int argc, char** argv)
{
    Solution s;
    //s.solve();
}
void Solution::calculatetemp()
{

}
void Solution::countRate()
{
    compressionRate = Strcode.length() / (originalStr.length() * 8);
    cout << "The compression rate is " << compressionRate << endl;
}
void judgeRelation()
{
}
int main()
{
    Solution s;
}
