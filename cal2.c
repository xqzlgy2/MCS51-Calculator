/**************************************************************************************
*                     四则运算计算器(带括号)                                              *
*                                                                                     *
*     连接方法：Jp8连JP4,JP9连JP5,把1602液晶插入                                           *
*                                                                                     *
***************************************************************************************/
        
#include <reg51.h>     //此文件中定义了51的一些特殊功能寄存器
#define uchar unsigned char
#define uint unsigned int
#define maxn 16
        
sbit EN = P2^7;  //LCD的使能引脚
sbit RS = P2^6;  //LCD数据命令选择端
sbit RW = P2^5;  //LCD的读写选择端

//3X4矩阵键盘键值表
uchar KEY_CODE[] = { 0xed,0xdd,0xbd,0x7d,
                     0xeb,0xdb,0xbb,0x7b,
                     0xe7,0xd7,0xb7,0x77};
//定义字符键值表
uchar CHAR_TABLE[] = {0x30,0x31,0x32,0x33, //0 1 2 3
                      0x34,0x35,0x36,0x37, //4 5 6 7 
                      0x38,0x39,0x28,0x29, //8 9 ( )
                      0x2B,0x2D,0x2A,0x2F};//+ - * /
                                           //M m C =
//错误信息
uchar MATH_ERROR[] = {0x45, 0x52, 0x4F};

uchar BUFFER[16]={' '};//定义长度等于液晶屏一行的缓冲区
uchar length = 0;      //当前缓冲区中字符个数
int result;
uchar ch;
int memory;
                
void scanChar(uchar *ch);//从矩阵键盘中获取值
void print(uchar *arr,uint end);//打印字符串
void delay5MS();//短延时函数
void delay100MS();//较长延时函数
void writeCMD(uchar com);//写命令子程序
void showOneChar(uchar dat);//写数据子程序
void init();//初始化子程序，初始化液晶显示屏
void clear();//清除显示屏上的显示

//将小数转换为字符串
int fabs(int x);
void toString();
void clearAll();
void showError();
int calculate();
int calc(int num1, int num2, char op);
int priority(char op);
void addBuffer();
        
void main()
{
    init();
    length = 0;
    result = 0;
    while (1)
    {
        ch = 0xff;      
        scanChar(&ch);
        if (length <= 16)
            BUFFER[length++] = ch;
        if (ch == '=')
        {
            length--;
            result = calculate();
            toString();
        }
        else if (ch == 'C')
            clearAll();
        else if (ch == 'M')
        {
            length--;
            memory = result;
            clearAll();
        }
        else if (ch == 'm')
        {
            length--;
            addBuffer();
        }
        clear();
        print(BUFFER,length);
    }
}

void addBuffer()
{
    int temp = length;
    uchar tmp, i;
    while (memory > 0)
    {
        BUFFER[length++] = memory % 10;
        memory /= 10;
    }
    if (length - temp > 1)
    {
        for(i = temp; i < temp+(length - temp)/2; i++)
        {
            tmp = BUFFER[i];
            BUFFER[i] = BUFFER[length-1-i+temp];
            BUFFER[length-1-i+temp] = tmp;
        }
    }
}

int priority(char op)
{
    if (op == '+' || op == '-')
        return 3;
    else if (op == '*' || op == '/')
        return 2;
    else if (op == '(')
        return 10;
    else
        return 0;
}

int calc(int num1, int num2, char op)
{
    switch (op)
    {
        case '+':
            return num1+num2;
            break;
        case '-':
            return num1-num2;
            break;
        case '*':
            return num1*num2;
            break;
        case '/':
            return num1/num2;
            break;
    }
    return 0;
}

int calculate()
{
    int oprand[maxn];
    char opStack[maxn];
    int x, y;
    char prei = 10, op, i, oprandi, opi;
    oprandi = 0;
    opi = 0;

    for(i = 0; i < length; i++)
    {
        if(BUFFER[i] >= 0 && BUFFER[i] <= 9)
        {
            int total = 0;
            int digit;
            while (BUFFER[i] >= 0 && BUFFER[i] <= 9)
            {
                digit = BUFFER[i];
                total = total*10 + digit;
                i++;
            }
            i--;
            oprand[oprandi++] = total;
        }
        else if(BUFFER[i]=='(')
            opStack[opi++] = BUFFER[i];
        else if(BUFFER[i]==')')
        {
            while(opStack[opi-1] != '(')
            {
                y = oprand[oprandi-1];    --oprandi;
                x = oprand[oprandi-1];    --oprandi;
                op = opStack[opi-1];    --opi;
                oprand[oprandi++] = calc(x,y,op);
            }
            --opi;
        }
        else if(BUFFER[i]=='-' && (prei==10 || !(prei >=0 && prei <= 9)))
        {
            oprand[oprandi++] = 0;
            opStack[opi++] = '-';
        }
        else
        {
            while(opi>0 && priority(BUFFER[i]) >= priority(opStack[opi-1]))
            {
                y = oprand[oprandi-1];    --oprandi;
                x = oprand[oprandi-1];    --oprandi;
                op = opStack[opi-1];    --opi;
                oprand[oprandi++] = calc(x,y,op);
            }
            opStack[opi++] = BUFFER[i];
        }
        prei = BUFFER[i];
    }

    while(opi>0)
    {
        y = oprand[oprandi-1];    --oprandi;
        x = oprand[oprandi-1];    --oprandi;
        op = opStack[opi-1];    --opi;
        oprand[oprandi++] = calc(x,y,op);
    }
    return oprand[oprandi-1];
}

int fabs(int x)
{
    return x > 0 ? x : -x;
}

void toString()
{
    uchar i, tmp;
    int temp;
    if (fabs(result) > 1000000)
    {
        showError();
        return;
    }
    temp = fabs(result);
    length = 0;
    if (temp == 0)
        BUFFER[length++] = 0;
    while (temp > 0)
    {
        BUFFER[length++] = temp % 10;
        temp /= 10;
    }
    if (result < 0)
        BUFFER[length++] = '-';
    if (length > 1)
    {
        for(i = length-1; i >= length / 2; i--)
        {
            tmp = BUFFER[i];
            BUFFER[i] = BUFFER[length-1-i];
            BUFFER[length-1-i] = tmp;
        }
    }
}
    
//从键盘扫描一个字符             
void scanChar(uchar *ch)
{
    uchar temp,num;
    uchar i = 1;
    temp = i;
    while(1)
    {
        //扫描独立按键K1~K8
        P3 = 0xff;
        delay100MS(); //延时，软件消除抖动
        if (P3 == 0xfe)
        {
            *ch = '+';
            break;
        }
        if (P3 == 0xfd)
        {
            *ch = '-';
            break;
        }
        if (P3 == 0xfb)
        {
            *ch = '*';
            break;
        }
        if (P3 == 0xf7)
        {
            *ch = '/';
            break;
        }
        if (P3 == 0xef)
        {
            *ch = 'M';
            break;
        }
        if (P3 == 0xdf)
        {
            *ch = 'm';
            break;
        }
        if (P3 == 0xbf)
        {
            *ch = 'C';
            break;
        }
        if (P3 == 0x7f)
        {
            *ch = '=';
            break;
        }           

        //扫描矩阵键盘
        P1 = 0x0f;//置行为高电平，列为低电平。这样用于检测行值。
        if (P1 != 0x0f)
        {                                           
            delay100MS(); //延时，软件消除抖动
            temp = P1; //保存行值
            P1 = 0xf0; //置行为低电平，列为高电平，获取列
            if (P1 != 0xf0)
            {
                num = temp|P1; //获取了按键位置
                for (i = 0; i < 12; i++)
                    if (num == KEY_CODE[i])
                    {
                       if (i == 10)
                           *ch = '(';
                       else if (i == 11)
                           *ch = ')';
                       else
                           *ch = i;
                    }
                break;  //跳出循环，为了只获取一个值
            }
        }    
    }
}
        
//延时5ms
void delay5MS()
{
    int n = 3000;
    while (n--);
}

//延时100ms 
void delay100MS()
{
    uint n = 10000;
    while (n--);
}

//向1602写命令
void writeCMD(uchar com)
{
    P0 = com;    //com为输入的命令码。通过P2送给LCD
    RS = 0;      //RS=0 写命令
    RW = 0;
    delay5MS();   
    EN = 1;      //LCD的使能端E置高电平
    delay5MS();
    EN = 0;      //LCD的使能端E置低电平   
}

//向1602写数据
void showOneChar(uchar dat)
{
    P0 = dat;     //写入数据
    RS = 1;       //RS=1写命令
    RW = 0;
    EN = 1;
    delay5MS();
    EN = 0;
      
}

//初始化液晶屏
void init()
{
    EN = 0;       
    writeCMD(0x38);//设置显示模式
    writeCMD(0x0e);//光标打开，不闪烁
    writeCMD(0x06);//写入一个字符后指针地址+1，写一个字符时整屏不移动
    writeCMD(0x01);//清屏显示，数据指针清0，所以显示清0
    writeCMD(0x80);//设置字符显示的首地址
}

//显示数组arr中end个字符
void print(uchar arr[],uint end)
{    
    uint t = 0, j = 0;
    uint location;
    if (end == 0 || end > 16) //若无字符或超出一行范围
    {
        clear();
        return;
    }
    else
    {
        for (t = 0; t < end; t++)
        {
            if (arr[t] == '(')
                location = 10;
            else if (arr[t] == ')')
                location = 11;
            else if (arr[t] == '+')
                location = 12;
            else if (arr[t] == '-')
                location = 13;
            else if (arr[t] == '*')
                location = 14;
            else if (arr[t] == '/')
                location = 15;
            else
            {
                for (j = 0; j < 10; j++)
                    if (arr[t] == j)
                        location = j;
            }
            showOneChar(CHAR_TABLE[location]);
        }
    }     
}

//清屏子程序
void clear()
{
    EN=0;
    writeCMD(0x01);
}

//重置
void clearAll()
{
    length = 0;
    clear();
}

//显示错误信息
void showError()
{
    clear();
    showOneChar(MATH_ERROR[0]);
    showOneChar(MATH_ERROR[1]);
    showOneChar(MATH_ERROR[1]);
    showOneChar(MATH_ERROR[2]);
    showOneChar(MATH_ERROR[1]);
    while (1)
    {
        ch = 0xff;        
        scanChar(&ch);
        if (ch == 'C')
        {
            clearAll();
            break;
        }
    }
}
