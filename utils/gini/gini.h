#ifndef GINI_H
#define GINI_H

#include <unordered_map>
#include <vector>



namespace glib{
class ParseIni{
public:
    ParseIni(char* filepath,char* filename);
    class IniValue{
    public:
        IniValue(){};
        IniValue(char* s_value):_inner_string_value(s_value){};
        IniValue(const std::string& s_value):_inner_string_value(s_value){};
        std::string to_string(){
            return _inner_string_value;
        }
        int to_int(){
            return atoi(_inner_string_value.c_str());
        }
        bool to_boolean(){
            return _inner_string_value=="true";
        }
    private:
        std::string _inner_string_value;
    };
    std::unordered_map<std::string,IniValue>& operator[](std::string& section_name);
    std::unordered_map<std::string,IniValue>& operator[](const char* section_name);
private:
    std::unordered_map<std::string,std::unordered_map<std::string,IniValue>> _top_values;
    // -1 注释或空行
    // 0 k=v 内容
    // 1 节名
    int judge_type(std::string& line);
};
}


#endif