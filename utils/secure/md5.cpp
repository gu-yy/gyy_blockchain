#include <iostream>
#include "md5.h"

namespace glib{
// 正弦函数表的64个常量值
static const std::uint32_t k[]={
        0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
        0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
        0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
        0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
        0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
        0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
        0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
        0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391,
};

// 每一轮循环左移的位数
static const int round1[]={7,12,17,22};
static const int round2[]={5,9,14,20};
static const int round3[]={4,11,16,23};
static const int round4[]={6,10,15,21};

// 循环左移函数
static std::uint32_t loop_left_move(std::uint32_t num,int r){
    std::uint32_t tmp=0x80000000;
    for(int i=0;i<r;++i){
        if((num&tmp)!=0){
            num=(num<<1)+1;
        }else{
            num=num<<1;
        }
    }
    return num;
}

// 线性变换函数
static std::uint32_t F(std::uint32_t X,std::uint32_t Y,std::uint32_t Z){return (X&Y)|((~X)&Z);}
static std::uint32_t G(std::uint32_t X,std::uint32_t Y,std::uint32_t Z){return (X&Z)|(Y&(~Z));}
static std::uint32_t H(std::uint32_t X,std::uint32_t Y,std::uint32_t Z){return X^Y^Z;}
static std::uint32_t I(std::uint32_t X,std::uint32_t Y,std::uint32_t Z){return Y^(X|(~Z));}

typedef std::uint32_t (*line_func)(std::uint32_t,std::uint32_t,std::uint32_t);
static std::uint32_t AA(std::uint32_t a,std::uint32_t b,std::uint32_t c,std::uint32_t d,
                        std::uint32_t message,int r,std::uint32_t k,line_func func){
    std::uint32_t first=a+func(b,c,d)+message+k;
    std::uint32_t second=loop_left_move(first,r);
    return second;
}

static std::unordered_map<int,char> num2char{
        {0,'0'},{1,'1'},{2,'2'},{3,'3'},
        {4,'4'},{5,'5'},{6,'6'},{7,'7'},
        {8,'8'},{9,'9'},{10,'a'},{11,'b'},
        {12,'c'},{13,'d'},{14,'e'},{15,'f'}
};

static std::string ui32tohexstr(std::uint32_t num){
    std::string hex_str(8,0);
    for(int i=0;i<4;++i){
        char c=0;
        c=num|c;
        char n=0x0f;
        char c1=num2char[c&n];
        char c2=num2char[(c>>4)&n];
        hex_str[7-i*2]=c1;
        hex_str[7-i*2-1]=c2;
        num>>=8;
    }
    return hex_str;
}

static std::string ui64tostr(std::uint64_t num){
    std::string buf(8,0);
    for(int i=0;i<8;++i){
        char c=0;
        buf[7-i]=num|c;
        num>>=8;
    }
    return buf;
}

static std::uint32_t strtoui32(std::string s){
    std::uint32_t num=0;
    for(int i=0;i<4;++i){
        num=num<<8;
        num=num|s[i];
    }
    return num;
}

static std::uint32_t A=0x67452301;
static std::uint32_t B=0xEFCDAB89;
static std::uint32_t C=0x98BADCFE;
static std::uint32_t D=0x10325476;

static void unit_hash(std::uint32_t* a,std::uint32_t* b,std::uint32_t* c,std::uint32_t* d,std::string s){
    std::vector<std::uint32_t> M(16);
    for(int i=0;i<16;++i){
        M[i]=strtoui32(s.substr(i*4,4));
    }
    std::uint32_t aa=*a;
    std::uint32_t bb=*b;
    std::uint32_t cc=*c;
    std::uint32_t dd=*d;
    // 分为以下四轮哈希，每一轮计算16次，一共64次
    // 第一轮
    *a=AA(*a,*b,*c,*d,M[0],round1[0],k[0],F);
    *d=AA(*d,*a,*b,*c,M[1],round1[1],k[1],F);
    *c=AA(*c,*d,*a,*b,M[2],round1[2],k[2],F);
    *b=AA(*b,*c,*d,*a,M[3],round1[3],k[3],F);
    *a=AA(*a,*b,*c,*d,M[4],round1[0],k[4],F);
    *d=AA(*d,*a,*b,*c,M[5],round1[1],k[5],F);
    *c=AA(*c,*d,*a,*b,M[6],round1[2],k[6],F);
    *b=AA(*b,*c,*d,*a,M[7],round1[3],k[7],F);
    *a=AA(*a,*b,*c,*d,M[8],round1[0],k[8],F);
    *d=AA(*d,*a,*b,*c,M[9],round1[1],k[9],F);
    *c=AA(*c,*d,*a,*b,M[10],round1[2],k[10],F);
    *b=AA(*b,*c,*d,*a,M[11],round1[3],k[11],F);
    *a=AA(*a,*b,*c,*d,M[12],round1[0],k[12],F);
    *d=AA(*d,*a,*b,*c,M[13],round1[1],k[13],F);
    *c=AA(*c,*d,*a,*b,M[14],round1[2],k[14],F);
    *b=AA(*b,*c,*d,*a,M[15],round1[3],k[15],F);
    // 第二轮
    *a=AA(*a,*b,*c,*d,M[1],round2[0],k[16],G);
    *d=AA(*d,*a,*b,*c,M[6],round2[1],k[17],G);
    *c=AA(*c,*d,*a,*b,M[11],round2[2],k[18],G);
    *b=AA(*b,*c,*d,*a,M[0],round2[3],k[19],G);
    *a=AA(*a,*b,*c,*d,M[5],round2[0],k[20],G);
    *d=AA(*d,*a,*b,*c,M[10],round2[1],k[21],G);
    *c=AA(*c,*d,*a,*b,M[15],round2[2],k[22],G);
    *b=AA(*b,*c,*d,*a,M[4],round2[3],k[23],G);
    *a=AA(*a,*b,*c,*d,M[9],round2[0],k[24],G);
    *d=AA(*d,*a,*b,*c,M[14],round2[1],k[25],G);
    *c=AA(*c,*d,*a,*b,M[3],round2[2],k[26],G);
    *b=AA(*b,*c,*d,*a,M[8],round2[3],k[27],G);
    *a=AA(*a,*b,*c,*d,M[13],round2[0],k[28],G);
    *d=AA(*d,*a,*b,*c,M[2],round2[1],k[29],G);
    *c=AA(*c,*d,*a,*b,M[7],round2[2],k[30],G);
    *b=AA(*b,*c,*d,*a,M[12],round2[3],k[31],G);
    // 第三轮
    *a=AA(*a,*b,*c,*d,M[5],round3[0],k[32],H);
    *d=AA(*d,*a,*b,*c,M[8],round3[1],k[33],H);
    *c=AA(*c,*d,*a,*b,M[11],round3[2],k[34],H);
    *b=AA(*b,*c,*d,*a,M[14],round3[3],k[35],H);
    *a=AA(*a,*b,*c,*d,M[1],round3[0],k[36],H);
    *d=AA(*d,*a,*b,*c,M[4],round3[1],k[37],H);
    *c=AA(*c,*d,*a,*b,M[7],round3[2],k[38],H);
    *b=AA(*b,*c,*d,*a,M[10],round3[3],k[39],H);
    *a=AA(*a,*b,*c,*d,M[13],round3[0],k[40],H);
    *d=AA(*d,*a,*b,*c,M[0],round3[1],k[41],H);
    *c=AA(*c,*d,*a,*b,M[3],round3[2],k[42],H);
    *b=AA(*b,*c,*d,*a,M[6],round3[3],k[43],H);
    *a=AA(*a,*b,*c,*d,M[9],round3[0],k[44],H);
    *d=AA(*d,*a,*b,*c,M[12],round3[1],k[45],H);
    *c=AA(*c,*d,*a,*b,M[15],round3[2],k[46],H);
    *b=AA(*b,*c,*d,*a,M[2],round3[3],k[47],H);
    // 第四轮
    *a=AA(*a,*b,*c,*d,M[0],round4[0],k[0],I);
    *d=AA(*d,*a,*b,*c,M[7],round4[1],k[1],I);
    *c=AA(*c,*d,*a,*b,M[14],round4[2],k[2],I);
    *b=AA(*b,*c,*d,*a,M[5],round4[3],k[3],I);
    *a=AA(*a,*b,*c,*d,M[12],round4[0],k[4],I);
    *d=AA(*d,*a,*b,*c,M[3],round4[1],k[5],I);
    *c=AA(*c,*d,*a,*b,M[10],round4[2],k[6],I);
    *b=AA(*b,*c,*d,*a,M[1],round4[3],k[7],I);
    *a=AA(*a,*b,*c,*d,M[8],round4[0],k[8],I);
    *d=AA(*d,*a,*b,*c,M[15],round4[1],k[9],I);
    *c=AA(*c,*d,*a,*b,M[6],round4[2],k[10],I);
    *b=AA(*b,*c,*d,*a,M[13],round4[3],k[11],I);
    *a=AA(*a,*b,*c,*d,M[4],round4[0],k[12],I);
    *d=AA(*d,*a,*b,*c,M[11],round4[1],k[13],I);
    *c=AA(*c,*d,*a,*b,M[2],round4[2],k[14],I);
    *b=AA(*b,*c,*d,*a,M[9],round4[3],k[15],I);

    *a+=aa;
    *b+=bb;
    *c+=cc;
    *d+=dd;
}

std::string md5(const std::string& text){
    std::string str(text);
    int length=str.size();
    length*=8;
    // 填充为N*512+448
    char first=0x80;
    str.push_back(first);
    length+=8;
    int left=length%512;
    int dif=0;
    if(left<448){
        dif=(448-left)/8;
    }else if(left>448){
        dif=(960-left)/8;
    }
    std::string padding(dif,0);
    str+=padding;
    std::uint64_t src_length=text.size();
    // 填充长度
    str+=ui64tostr(src_length);
    length=str.size();
    // 每次取512位，一共取N+1次
    int idx=0;
    std::uint32_t a=A;
    std::uint32_t b=B;
    std::uint32_t c=C;
    std::uint32_t d=D;
    while(idx<length){
        std::string sub_str=str.substr(idx,64);
        idx+=64;
        unit_hash(&a,&b,&c,&d,sub_str);
    }
    return ui32tohexstr(a)+ui32tohexstr(b)+ui32tohexstr(c)+ui32tohexstr(d);
}
}