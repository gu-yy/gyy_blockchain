#include "encode.h"

namespace glib{

void EncodeInt2CharArray(char* dst,uint32_t num){
    for(int i=0;i<4;++i){
        *(dst++)=0xFF&num;
        num>>=8;
    }
}

uint32_t DecodeCharArray2Int(const char* src){
    uint32_t res=0;
    for(int i=0;i<4;++i){
        int step=8;
        res=res | (*(src+i)&0xFF)<<step*i;
    }

    return res;
}
}