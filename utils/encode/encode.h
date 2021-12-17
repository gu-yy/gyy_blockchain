#ifndef ENCODE_H
#define ENCODE_H
#include <cstdint>

namespace glib{
void EncodeInt2CharArray(char* dst,uint32_t num);
uint32_t DecodeCharArray2Int(const char* src);
}

#endif