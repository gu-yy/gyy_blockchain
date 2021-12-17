#include <iostream>
#include "grena.h"

namespace glib{
Grena* Grena::_grena=nullptr;
Grena::Grena(std::size_t bytes):_max_size(bytes),_used_size(0),_mem_pool(nullptr),_cur_ptr(nullptr){
    _mem_pool=new char[bytes];
    _cur_ptr=_mem_pool;
}

Grena::~Grena(){
    delete _mem_pool;
    _mem_pool=nullptr;
    _cur_ptr=nullptr;
    _max_size=0;
    _used_size=0;
}

void Grena::swap(Grena& rhs){
    std::swap(_mem_pool,rhs._mem_pool);
    std::swap(_cur_ptr,rhs._cur_ptr);
    std::swap(_max_size,rhs._max_size);
    std::swap(_used_size,rhs._used_size);
}

char* Grena::allocate(std::size_t bytes){
    if(_used_size+bytes>_max_size){
        return nullptr;
    }
    char* tmp=_cur_ptr;
    _cur_ptr+=bytes;
    _used_size+=bytes;
    return tmp;
}
}