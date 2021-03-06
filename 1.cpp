/*************************************************************************
	> File Name: main.cpp
	> Author: chudongfang
	> Git: https://github.com/chudongfang
	> Created Time: 2018年04月10日 星期二 11时21分47秒
 ************************************************************************/

#include<iostream>
#include<string>
#include<cstdlib>
#include<fstream>
#include<sstream>
#include<set>
#include<map>
#include<vector>

using namespace std;


class Compiler
{
public:
    Compiler()
    {
        contx_="";
        filecontx_="";

    };
    bool IsLetter(char ch);
    bool IsDigit(char ch);
    bool readFile(const string FileName);
    void preProcessing();
    int  findReserve(const string str);
    void Scanner(int &syn ,string &str , int &beginPoint);
    bool Hander();
    void setFileName(string FileName)
    { FileName_ = FileName; }

private:
    string contx_;
    string filecontx_;
    string FileName_;
     
    set<string> identifierTb; // 标识符表
    vector<string> identifier; // 标识符
    vector<string> reserveWord;// 保留字
    vector<string> constant; //常数
    vector<string> operatorOrDelimiter;//运算符号

    static string operatorOrDelimiterList[36];
    static  string reserveWordList[32] ;
};

string Compiler::operatorOrDelimiterList[36] = {
        "+", "-", "*", "/", "<", "<=", ">", ">=", "=", "==",
        "!=", ";", "(", ")", "^", ",", "\"", "\'", "#", "&",
        "&&", "|", "||", "%", "~", "<<", ">>", "[", "]", "{",
        "}", "\\", ".", "\?", ":", "!"
};

string Compiler::reserveWordList[32] = {
      "auto", "break", "case", "char", "const", "continue",
      "default", "do", "double", "else", "enum", "extern",
      "float", "for", "goto", "if", "int", "long",
      "register", "return", "short", "signed", "sizeof", "static",
      "struct", "switch", "typedef", "union", "unsigned", "void",
      "volatile", "while"
};

bool Compiler::readFile(const string FileName)
{
    fstream in(FileName.c_str());
    if(!in.is_open())
    {
        cout << "EROOR : can't not open"<<endl;
        return false;
    }

    ostringstream tmp; 
    tmp << in.rdbuf(); 

    this->filecontx_ = tmp.str();
    
    //debug
    cout << filecontx_<<endl;

    return true;
}














bool Compiler::IsLetter(char ch)
{
    return (ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z' || ch == '_');
}

bool Compiler::IsDigit(char ch)
{
    return (ch >= '0' && ch <= '9');
}





void Compiler::preProcessing()
{
    string s = this->filecontx_;
    string ans=""; 
    int siz = s.size();
    for(int i = 0 ; i < siz; i++)
    {

        //  //#########
        if(s[i]=='/' && s[i+1] == '/')
        {
            while(s[i]!='\n')
            {
                i++;
            }
        }
        

        // /*###########*/
        if( s[i]=='/' && s[i+1] == '*')
        {
            i+=2;

            while(s[i]!='*' || s[i+1] != '/')
            {
                i++;
                if( s[i] == '$' )
                {
                    cout << "EROOR : /* */"<<endl;
                    exit(0);
                }
            }
            i+=2;

        }

        
        // dell with \n \r \t \v
        if(s[i] != '\n' && s[i] != '\t' && s[i] != '\v' && s[i] != '\r')
        {
            ans += s[i];
        }
    }

    this->contx_ = ans;
    //debug
    cout << this->contx_<<endl;

}




int Compiler::findReserve(const string str)
{
    for(int i=0; i < 32; i++)
    {
        if(reserveWordList[i] == str)
        {
            return i+1;
        }
    }
    return -1;
}




void Compiler::Scanner(int &syn , string &str , int &beginPoint)
{

    int i;
    char first;

    first = contx_[beginPoint];
    while(first == ' ')
    {
        first = contx_[++beginPoint];
    }
    
    str = "";
    

    if( IsLetter(first) )
    {
        str += first;

        beginPoint++;
        while( IsLetter( contx_[beginPoint] ) || IsDigit( contx_[beginPoint] ) ) 
        {
            str += contx_[beginPoint++];
        }
        
        
        //syn =  findReserve(reserveWord,tock);
        
        //如果不是保留字，则是标识符
        if(syn == -1)
        {
            syn = 100;
        }
        return ;
    }
    else if (IsDigit( first ))
    {
        while(IsDigit(contx_[beginPoint]))
        {
            str +=  contx_[beginPoint++];
        }
        syn = 99;

    }
    else if( first == '+' || first == '-' || first == '*' || first == '/' || 
             first == ';' || first == '(' || first == ')' || first == '^' ||
             first == ',' || first == '\"' || first == '\'' || first == '~' ||
             first == '#' || first == '%' || first == '[' || first == ']' || 
             first == '{' || first == '}' || first == '\\' || first == '.' || 
            first == '\?' || first == ':')
    {
        str += contx_[beginPoint];
 
        
        for (i = 0; i<36; i++)
        {
            //查运算符界符表
            if (str == operatorOrDelimiterList[i])
            {
                syn = 33 + i;//获得种别码，使用了一点技巧，使之呈线性映射
                break;//查到即推出
            }
        }
        
        beginPoint++;//指针下移，为下一扫描做准备
        return ;

    }
    //FIXME  
    else  if (first == '<')
    {//<,<=,<<
        beginPoint++;//后移，超前搜索
        if (contx_[beginPoint] == '=')
        {
            str = "<=";
            syn = 38;
        }
        else if (contx_[beginPoint] == '<')
        {//左移
            str = "<<";
            syn = 58;
        }
        else
        {
            beginPoint--;
            str="<";
            syn = 37;
        }
        beginPoint++;//指针下移
        return;
    }
    else  if (first == '>')
    {//>,>=,>>
        beginPoint++;
        if (contx_[beginPoint] == '=')
        {
            str = ">=";
            syn = 40;
        }
        else if (contx_[beginPoint] == '>')
        {
            str = ">>";
            syn = 59;
        }
        else
        {
            beginPoint--;
            str = ">";
            syn = 39;
        }
        beginPoint++;
        return;
    }
    else  if (first == '=')
    {//=.==
        beginPoint++;
        if (contx_[beginPoint] == '=')
        {
            str = "==";
            syn = 42;
        }
        else
        {
            str = "=";
            beginPoint--;
            syn = 41;
        }
        beginPoint++;
        return;
    }
    else  if (first == '!')
    {//!,!=
        beginPoint++;
        if (contx_[beginPoint] == '=')
        {
            str = "!=";
            syn = 43;
        }
        else
        {
            str = "!";
            syn = 68;
            beginPoint--;
        }
        beginPoint++;
        return;
    }
    else  if (first == '&')
    {//&,&&
        beginPoint++;
        if (contx_[beginPoint] == '&')
        {
            str = "&&";
            syn = 53;
        }
        else
        {
            str = "&";
            beginPoint--;
            syn = 52;
        }
        beginPoint++;
        return;
    }
    else  if (first == '|')
    {//|,||
        beginPoint++;
        if (contx_[beginPoint] == '|')
        {
            str = "||";
            syn = 55;
        }
        else
        {
            str = "|";
            beginPoint--;
            syn = 54;
        }
        beginPoint++;
        return;
    }
    else if( first == '$' )
    {
        str = "$";
        syn = 0;   
    }
    else
    {
        cout << "EROOR : not exist!"<<endl;
        exit(0);
    }
    
    
}



bool Compiler::Hander()
{
    int syn = -1;
    int beginPoint = 0;
    string str;
    while(syn!=0)
    {
        Scanner(syn, str,beginPoint);
        if(syn == 100)
        {
            identifierTb.insert(str);
            identifier.push_back(str);
        }
        else if(syn>=1&&syn<=32)
        {
            reserveWord.push_back(str);
        }
        else if(syn == 99)
        {
            constant.push_back(str);
        }
        else if(syn >= 33 && syn <= 68)
        {
            operatorOrDelimiter.push_back(str);
        }
        
        //debug
        cout <<"-->"<< str<<endl;
        
    }



    for(set<string>::iterator it = identifierTb.begin(); it != identifierTb.end(); it++)
    {
       cout << *it<<endl; 
    }

}





int main()
{
    Compiler a ; 
    a.readFile("1.cpp");
    a.preProcessing();
    a.Hander();
    return 0;
}











