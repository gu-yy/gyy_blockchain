#ifndef GRENA_H
#define GRENA_H

#include <cstddef>


namespace glib{
class Grena{
public:
    static Grena* get_grena(std::size_t bytes){
        if(_grena==nullptr){
            _grena=new Grena(bytes);
        }
        return _grena;
    }
    void swap(Grena& rhs);
    char* allocate(std::size_t bytes);
    std::size_t get_used(){
        return _used_size;
    }
private:
    Grena(std::size_t bytes);
    Grena(const Grena&)=delete;
    Grena& operator=(const Grena&)=delete;
    ~Grena();
    char* _mem_pool;
    char* _cur_ptr;
    size_t _max_size;
    size_t _used_size;
    static Grena* _grena;
};
}

#endif