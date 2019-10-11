/**************************************************************************************
*                     浮点数四则运算计算器                                                  *
*                                                                                     *
*     连接方法：Jp8连JP4,JP9连JP5,把1602液晶插入                                          *
*                                                                                     *
***************************************************************************************/
        
#include <reg51.h>     //此文件中定义了51的一些特殊功能寄存器
#define uchar unsigned char
#define uint unsigned int
        
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
                      0x2B,0x2D,0x2A,0x2F, //+ - * /
                      0x2E,0x25,0x43,0x3D};//. % C =
//错误信息
uchar MATH_ERROR[] = {0x4D, 0x41, 0x54, 0x48, 0x20, 0x45, 0x52, 0x52, 0x4F, 0x52, 0x21};

uchar BUFFER[16]={' '};//定义长度等于液晶屏一行的缓冲区
uchar length = 0;      //当前缓冲区中字符个数
uchar status = 0;      //状态编号

xdata uchar stack1[64];
xdata float stack2[48];
xdata uchar pt1 = 0;
xdata uchar pt2 = 0;
                
void scanChar(uchar *ch);//从矩阵键盘中获取值
void print(uchar *arr,uint end);//打印字符串
void delay5MS();//短延时函数
void delay100MS();//较长延时函数
void writeCMD(uchar com);//写命令子程序
void showOneChar(uchar dat);//写数据子程序
void init();//初始化子程序，初始化液晶显示屏
void clear();//清除显示屏上的显示

float oprand1;
float oprand2;
float oprand3;
uchar op1, op2, op3, ch;
long weight;

//定义状态函数
void status0(); void status1();
void status2(); void status3();
void status4(); void status5();
void status6(); void status7();
void status8(); void status9();
void status10(); void status11();
void status12(); void status13();
void status14(); void status15();
void status16(); void status17();
void status18(); void status19();
void status20(); void status21();
void status22();

//将小数转换为字符串
void floatToString();
void clearAll();
float fabs(float x);
void sqrtNewton();
void showError();

void PushStack()
{
    stack1[pt1++] = status;
    stack1[pt1++] = op1;
    stack1[pt1++] = op2;
    stack1[pt1++] = op3;
    stack2[pt2++] = oprand1;
    stack2[pt2++] = oprand2;
    stack2[pt2++] = oprand3;
    oprand1 = oprand2 = oprand3 = 0;
    weight = 10;
    status = 0;
}

void PopStack()
{
    float temp;
    uchar temppt1;
    ch = 0xff;        
    scanChar(&ch);
    if (length <= 16)
        BUFFER[length++] = ch;
    if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '=' || ch == ')')
    {
        temppt1 = pt1;
        temp = oprand1;
        if (pt1 != 0)
        {
            op3 = stack1[--pt1];
            op2 = stack1[--pt1];
            op1 = stack1[--pt1];
            status = stack1[--pt1];
            oprand3 = stack2[--pt2];
            oprand2 = stack2[--pt2];
            oprand1 = stack2[--pt2];
        }
        if (status == 5)
        {
            oprand2 = temp;
            if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '=' || ch == ')')
            {
                op2 = ch;
                status = 16;
            }
        }
        else if (status == 6)
        {
            oprand3 = temp;
            if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '=' || ch == ')')
            {
                op2 = ch;
                status = 11;
            }
        }
        else if (status == 17)
        {
            oprand3 = temp;
            if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '=' || ch == ')')
            {
                op3 = ch;
                status = 22;
            }
        }
        else if (status == 0)
        {
            oprand1 = temp;
            if (ch == '+' || ch == '-')
            {
                oprand2 = 0;
                status = 5;
            }
            else if (ch == '*' || ch == '/')
            {
                oprand3 = 0;
                status = 6;
            }
        }

    }
    else
    {
        length--;
        status = 'm';
        return;
    }
}

void main()
{
    init();
    length = 0;
    status = 0;
    oprand1 = 0;
    while (1)
    {
        switch (status)
        {
            case 0:
                status0();
                break;
            case 1:
                status1();
                break;
            case 2:
                status2();
                break;
            case 3:
                status3();
                break;
            case 4:
                status4();
                break;
            case 5:
                status5();
                break;
            case 6:
                status6();
                break;
            case 7:
                status7();
                break;
            case 8:
                status8();
                break;
            case 9:
                status9();
                break;
            case 10:
                status10();
                break;
            case 11:
                status11();
                break;
            case 12:
                status12();
                break;
            case 13:
                status13();
                break;
            case 14:
                status14();
                break;
            case 15:
                status15();
                break;
            case 16:
                status16();
                break;
            case 17:
                status17();
                break;
            case 18:
                status18();
                break;
            case 19:
                status19();
                break;
            case 20:
                status20();
                break;
            case 21:
                status21();
                break;
            case 22:
                status22();
                break;
            case 'm':
                PopStack();  
        }
        clear();
        print(BUFFER,length);
    }
}

void floatToString()
{
    uchar i, tmp;
    long oprand1Int, floatInt;
    float temp, oprand1Float;
    if (fabs(oprand1) > 1000000000)
        showError();
    temp = oprand1 > 0 ? oprand1 : -oprand1;
    oprand1Int = temp;
    oprand1Float = temp - oprand1Int;
    oprand1Float *= 100000;
    floatInt = oprand1Float;
    length = 0;
    if (floatInt == 0)
        BUFFER[length++] = 0;
    while (floatInt > 0)
    {
        BUFFER[length++] = floatInt % 10;
        floatInt /= 10;
    }
    for (; length < 5; length++)
        BUFFER[length] = 0;
    BUFFER[length++] = '.';
    if (oprand1Int == 0)
        BUFFER[length++] = 0;
    while (oprand1Int > 0)
    {
        BUFFER[length++] = oprand1Int % 10;
        oprand1Int /= 10;
    }
    if (oprand1 < 0)
        BUFFER[length++] = '-';
    for(i = length-1; i >= length / 2; i--)
    {
        tmp = BUFFER[i];
        BUFFER[i] = BUFFER[length-1-i];
        BUFFER[length-1-i] = tmp;
    }
}


float fabs(float x)
{
    return x > 0 ? x : -x;
}
/*
void sqrtNewton()
{
    float x = oprand1;
    float y = 0.0;
    while (fabs(x-y) > 0.00001)
    {
        y = x;
        x = 0.5 * (x + oprand1/x);
    }
    oprand1 = x;
}*/

void status0()
{
    ch = 0xff;        
    scanChar(&ch);
    if (length <= 16)
        BUFFER[length++] = ch;
    if (ch == '-')
        status = 1;
    else if (ch >= 0 && ch <= 9)
    {
        oprand1 = ch;
        status = 2;
    }
    else if (ch == '.')
    {
        weight = 10;
        status = 4;
    }
    /*
    else if (ch == 'S')
    {
        if (oprand1 < 0)
            showError();
        else
        {
            sqrtNewton();
            floatToString();
        }
    }
    else if (ch == 'Q')
    {
        oprand1 *= oprand1;
        floatToString();
    }
    */
    else if (ch == '%')
    {
        if (oprand1 < 0)
            showError();
        else
        {
            oprand1 *= 100;
            floatToString();
        }
    }
    else if (ch == '(')
        PushStack();
    else if (ch == 'C')
        clearAll();
    else
        length--;
}

void status1()
{
    ch = 0xff;        
    scanChar(&ch);
    if (length <= 16)
        BUFFER[length++] = ch;
    if (ch >= 0 && ch <= 9)
    {
        oprand1 = oprand1 * 10 + ch;
        status = 1;
    }
    else if (ch == '.')
    {
        weight = 10;
        status = 3;
    }
    else if (ch == '+' || ch == '-')
    {
        oprand1 = -oprand1;
        oprand2 = 0;
        status = 5;
    }
    else if (ch == '*' || ch == '/')
    {
        oprand1 = -oprand1;
        oprand3 = 0;
        status = 6;
    }
    else if (ch == 'C')
        clearAll();
    else
        length--;
}

void status2()
{
    ch = 0xff;        
    scanChar(&ch);
    if (length <= 16)
        BUFFER[length++] = ch;
    if (ch >= 0 && ch <= 9)
    {
        oprand1 = oprand1 * 10 + ch;
        status = 2;
    }
    else if (ch == '.')
    {
        weight = 10;
        status = 4;
    }
    else if (ch == '+' || ch == '-')
    {
        oprand2 = 0;
        status = 5;
    }
    else if (ch == '*' || ch == '/')
    {
        oprand3 = 0;
        status = 6;
    }
    else if (ch == 'C')
        clearAll();
    else
        length--;
}

void status3()
{
    ch = 0xff;        
    scanChar(&ch);
    if (length <= 16)
        BUFFER[length++] = ch;
    if (ch >= 0 && ch <= 9)
    {
        oprand1 += ch/(float)weight;
        weight *= 10;
        status = 3;
    }
    else if (ch == '+' || ch == '-')
    {
        oprand1 = -oprand1;
        oprand2 = 0;
        status = 5;
    }
    else if (ch == '*' || ch == '/')
    {
        oprand1 = -oprand1;
        oprand3 = 0;
        status = 6;
    }
    else if (ch == 'C')
        clearAll();
    else
        length--;
}

void status4()
{
    ch = 0xff;        
    scanChar(&ch);
    if (length <= 16)
        BUFFER[length++] = ch;
    if (ch >= 0 && ch <= 9)
    {
        oprand1 += ch/(float)weight;
        weight *= 10;
        status = 4;
    }
    else if (ch == '+' || ch == '-')
    {
        oprand2 = 0;
        status = 5;
    }
    else if (ch == '*' || ch == '/')
    {
        oprand3 = 0;
        status = 6;
    }
    else if (ch == 'C')
        clearAll();
    else
        length--;
}

void status5()
{
    op1 = ch;
    ch = 0xff;        
    scanChar(&ch);
    if (length <= 16)
        BUFFER[length++] = ch;
    oprand2 = 0;
    if (ch == '-')
        status = 12;
    else if (ch >= 0 && ch <= 9)
    {
        oprand2 = ch;
        status = 13;
    }
    else if (ch == '.')
    {
        weight = 10;
        status = 15;
    }
    else if (ch == 'C')
        clearAll();
    else if (ch == '(')
        PushStack();
    else
        length--;
}

void status6()
{
    op1 = ch;
    ch = 0xff;        
    scanChar(&ch);
    if (length <= 16)
        BUFFER[length++] = ch;
    oprand3 = 0;
    if (ch == '-')
        status = 7;
    else if (ch >= 0 && ch <= 9)
    {
        oprand3 = ch;
        status = 8;
    }
    else if (ch == '.')
    {
        weight = 10;
        status = 10;
    }
    else if (ch == 'C')
        clearAll();
    else if (ch == '(')
        PushStack();
    else
        length--;
}

void status7()
{
    ch = 0xff;        
    scanChar(&ch);
    if (length <= 16)
        BUFFER[length++] = ch;
    if (ch >= 0 && ch <= 9)
    {
        oprand3 = oprand3 * 10 + ch;
        status = 7;
    }
    else if (ch == '.')
    {
        weight = 10;
        status = 9;
    }
    else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '=' || ch == ')')
    {
        oprand3 = -oprand3;
        op2 = ch;
        status = 11;
    }
    else if (ch == 'C')
        clearAll();
    else
        length--;
}

void status8()
{
    ch = 0xff;        
    scanChar(&ch);
    if (length <= 16)
        BUFFER[length++] = ch;
    if (ch >= 0 && ch <= 9)
    {
        oprand3 = oprand3 * 10 + ch;
        status = 8;
    }
    else if (ch == '.')
    {
        weight = 10;
        status = 10;
    }
    else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '=' || ch == ')')
    {
        op2 = ch;
        status = 11;
    }
    else if (ch == 'C')
        clearAll();
    else
        length--;
}

void status9()
{
    ch = 0xff;        
    scanChar(&ch);
    if (length <= 16)
        BUFFER[length++] = ch;
    if (ch >= 0 && ch <= 9)
    {
        oprand3 += ch/(float)weight;
        weight *= 10;
        status = 9;
    }
    else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '=' || ch == ')')
    {
        oprand3 = -oprand3;
        op2 = ch;
        status = 11;
    }
    else if (ch == 'C')
        clearAll();
    else
        length--;
}

void status10()
{
    ch = 0xff;        
    scanChar(&ch);
    if (length <= 16)
        BUFFER[length++] = ch;
    if (ch >= 0 && ch <= 9)
    {
        oprand3 += ch/(float)weight;
        weight *= 10;
        status = 10;
    }
    else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '=' || ch == ')')
    {
        op2 = ch;
        status = 11;
    }
    else if (ch == 'C')
        clearAll();
    else
        length--;
}

void status11()
{
    if (op1 == '*')
        oprand1 *= oprand3;
    else if (op1 == '/')
    {
        if (fabs(oprand3) < 0.00001)
            showError();
        oprand1 /= oprand3;
    }
    if (op2 == '=')
    {
        if (pt1 != 0)
            showError();
        else
        {
            floatToString();
            status = 0;
        }
    }
    else if (op2 == ')')
    {
        if (pt1 != 0)
            status = 'm';
        else
            showError();
    }
    else if (op2 == '*' || op2 == '/')
        status = 6;
    else if (op2 == '+' || op2 == '-')
        status = 5;
}

void status12()
{
    ch = 0xff;        
    scanChar(&ch);
    if (length <= 16)
        BUFFER[length++] = ch;
    if (ch >= 0 && ch <= 9)
    {
        oprand2 = oprand2 * 10 + ch;
        status = 12;
    }
    else if (ch == '.')
    {
        weight = 10;
        status = 14;
    }
    else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '=' || ch == ')')
    {
        oprand2 = -oprand2;
        op2 = ch;
        status = 16;
    }
    else if (ch == 'C')
        clearAll();
    else
        length--;
}

void status13()
{
    ch = 0xff;        
    scanChar(&ch);
    if (length <= 16)
        BUFFER[length++] = ch;
    if (ch >= 0 && ch <= 9)
    {
        oprand2 = oprand2 * 10 + ch;
        status = 13;
    }
    else if (ch == '.')
    {
        weight = 10;
        status = 15;
    }
    else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '=' || ch == ')')
    {
        op2 = ch;
        status = 16;
    }
    else if (ch == 'C')
        clearAll();
    else
        length--;
}

void status14()
{
    ch = 0xff;        
    scanChar(&ch);
    if (length <= 16)
        BUFFER[length++] = ch;
    if (ch >= 0 && ch <= 9)
    {
        oprand2 += ch/(float)weight;
        weight *= 10;
        status = 14;
    }
    else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '=' || ch == ')')
    {
        oprand2 = -oprand2;
        op2 = ch;
        status = 16;
    }
    else if (ch == 'C')
        clearAll();
    else
        length--;
}

void status15()
{
    ch = 0xff;        
    scanChar(&ch);
    if (length <= 16)
        BUFFER[length++] = ch;
    if (ch >= 0 && ch <= 9)
    {
        oprand2 += ch/(float)weight;
        weight *= 10;
        status = 15;
    }
    else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '=' || ch == ')')
    {
        op2 = ch;
        status = 16;
    }
    else if (ch == 'C')
        clearAll();
    else
        length--;
}

void status16()
{
    if (op2 == '=')
    {
        if (pt1 != 0)
            showError();
        else
        {
            if (op1 == '+')
                oprand1 += oprand2;
            else if (op1 == '-')
                oprand1 -= oprand2;
            floatToString();
            status = 0;
        }
    }
    else if (op2 == '*' || op2 == '/')
        status = 17;
    else if (op2 == '+' || op2 == '-')
    {
        if (op1 == '+')
            oprand1 += oprand2;
        else if (op1 == '-')
            oprand1 -= oprand2;
        status = 5;
    }
    else if (op2 == ')')
    {
        if (op1 == '+')
            oprand1 += oprand2;
        else if (op1 == '-')
            oprand1 -= oprand2;
        if (pt1 != 0)
            status = 'm';
        else
            showError();
    }
}

void status17()
{
    ch = 0xff;        
    scanChar(&ch);
    if (length <= 16)
        BUFFER[length++] = ch;
    oprand3 = 0;
    if (ch == '-')
        status = 18;
    else if (ch >= 0 && ch <= 9)
    {
        oprand3 = ch;
        status = 19;
    }
    else if (ch == '.')
    {
        weight = 10;
        status = 21;
    }
    else if (ch == 'C')
        clearAll();
    else if (ch == '(')
        PushStack();
    else
        length--;
}

void status18()
{
    ch = 0xff;        
    scanChar(&ch);
    if (length <= 16)
        BUFFER[length++] = ch;
    if (ch >= 0 && ch <= 9)
    {
        oprand3 = oprand3 * 10 + ch;
        status = 18;
    }
    else if (ch == '.')
    {
        weight = 10;
        status = 20;
    }
    else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '=' || ch == ')')
    {
        oprand3 = -oprand3;
        op3 = ch;
        status = 22;
    }
    else if (ch == 'C')
        clearAll();
    else
        length--;
}

void status19()
{
    ch = 0xff;        
    scanChar(&ch);
    if (length <= 16)
        BUFFER[length++] = ch;
    if (ch >= 0 && ch <= 9)
    {
        oprand3 = oprand3 * 10 + ch;
        status = 19;
    }
    else if (ch == '.')
    {
        weight = 10;
        status = 21;
    }
    else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '=' || ch == ')')
    {
        op3 = ch;
        status = 22;
    }
    else if (ch == 'C')
        clearAll();
    else
        length--;
}

void status20()
{
    ch = 0xff;        
    scanChar(&ch);
    if (length <= 16)
        BUFFER[length++] = ch;
    if (ch >= 0 && ch <= 9)
    {
        oprand3 += ch/(float)weight;
        weight *= 10;
        status = 20;
    }
    else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '=' || ch == ')')
    {
        oprand3 = -oprand3;
        op3 = ch;
        status = 22;
    }
    else if (ch == 'C')
        clearAll();
    else
        length--;
}

void status21()
{
    ch = 0xff;        
    scanChar(&ch);
    if (length <= 16)
        BUFFER[length++] = ch;
    if (ch >= 0 && ch <= 9)
    {
        oprand3 += ch/(float)weight;
        weight *= 10;
        status = 21;
    }
    else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '=' || ch == ')')
    {
        op3 = ch;
        status = 22;
    }
    else if (ch == 'C')
        clearAll();
    else
        length--;
}

void status22()
{
    if (op2 == '/' && fabs(oprand3) < 0.00001)
            showError();
    if (op3 == '=')
    {
        if (pt1 != 0)
            showError();
        else
        {
            if (op1 == '+' && op2 == '*')
                oprand1 = oprand1 + oprand2 * oprand3;
            else if (op1 == '-' && op2 == '*')
                oprand1 = oprand1 - oprand2 * oprand3;
            else if (op1 == '+' && op2 == '/')
                oprand1 = oprand1 + oprand2 / oprand3;
            else if (op1 == '-' && op2 == '/')
                oprand1 = oprand1 - oprand2 / oprand3;
            floatToString();
            status = 0;
        }
    }
    else if (op3 == ')')
    {
        if (op1 == '+' && op2 == '*')
            oprand1 = oprand1 + oprand2 * oprand3;
        else if (op1 == '-' && op2 == '*')
            oprand1 = oprand1 - oprand2 * oprand3;
        else if (op1 == '+' && op2 == '/')
            oprand1 = oprand1 + oprand2 / oprand3;
        else if (op1 == '-' && op2 == '/')
            oprand1 = oprand1 - oprand2 / oprand3;
        if (pt1 != 0)
            status = 'm';
        else
            showError();
    }
    else if (op3 == '*' || op3 == '/')
    {
        if (op2 == '*')
            oprand2 *= oprand3;
        if (op2 == '/')
            oprand2 /= oprand3;
        op2 = op3;
        status = 17;
    }
    else if (op3 == '+' || op3 == '-')
    {
        if (op1 == '+' && op2 == '*')
            oprand1 = oprand1 + oprand2 * oprand3;
        else if (op1 == '-' && op2 == '*')
            oprand1 = oprand1 - oprand2 * oprand3;
        else if (op1 == '+' && op2 == '/')
            oprand1 = oprand1 + oprand2 / oprand3;
        else if (op1 == '-' && op2 == '/')
            oprand1 = oprand1 - oprand2 / oprand3;
        op1 = op3;
        status = 5;
    }
}
    
//从键盘扫描一个字符             
void scanChar(uchar *ch)
{
    uchar temp,num;
    int i = 1;
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
            *ch = '.';
            break;
        }
        if (P3 == 0xdf)
        {
            *ch = '%';
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
            else if (arr[t] == '.')
                location = 16;
            else if (arr[t] == '%')
                location = 17;
            else if (arr[t] == 'C')
                location = 18;
            else if (arr[t] == '=')
                location = 19;
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
    oprand1 = oprand2 = oprand3 = 0;
    length = 0;
    weight = 10;
    status = 0;
    pt1 = pt2 = 0;
    clear();
}

//显示错误信息
void showError()
{
    uchar i;
    clear();
    for (i = 0; i < 11; i++)
        showOneChar(MATH_ERROR[i]);
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