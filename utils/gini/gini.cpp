#include <fstream>
#include <cstring>

#include "gini.h"

namespace glib{

static std::string trim_space(std::string& line){
    while(line.back()=='\r' || line.back()=='\r' || line.back()==' '){
        line.pop_back();
    }
    size_t idx=0;
    while(idx<line.size() && line[idx]==' '){
        ++idx;
    }
    line=line.substr(idx);
    if(line.front()=='"'&&line.back()=='"'){
        line=line.substr(1,line.size()-2);
    }
    return line;
}

static std::string get_section_name(std::string& line){
    std::string section_name;
    for(char c:line){
        if(c!='[' && c!=']'){
            section_name.push_back(c);
        }
    }
    return section_name;
}

static std::pair<std::string,std::string> get_key_value(std::string& line){
    std::pair<std::string,std::string> kv;
    bool flag=false;
    for(char c:line){
        if((!flag)&&c=='='){
            flag=true;
        }else{
            if(!flag){
                kv.first.push_back(c);
            }else{
                kv.second.push_back(c);
            }
        }
    }
    kv.first=trim_space(kv.first);
    kv.second=trim_space(kv.second);
    return kv;
}

ParseIni::ParseIni(char* filepath,char* filename){
    std::string fullname=std::string(filepath)+std::string(filename);
    std::string section_name;
    std::ifstream inifile(fullname);
        if(inifile.is_open()){
            std::string line;
            while(std::getline(inifile,line)){
                int type=judge_type(line);
                if(type==1){
                    section_name=get_section_name(line);
                }else if(type==0){
                    std::pair<std::string,std::string> kv=get_key_value(line);
                    _top_values[section_name][kv.first]=kv.second;
                }
            }
            inifile.close();
        }
}

int ParseIni::judge_type(std::string& line){
    std::string new_line=trim_space(line);
    if(new_line.size()==0){
        return -1;
    }
    if(line.front()=='#'){
        return -1;
    }else if(line.front()=='['){
        return 1;
    }else{
        return 0;
    }
}

std::unordered_map<std::string,ParseIni::IniValue>& ParseIni::operator[](std::string& section_name){
    return _top_values[section_name];
}
std::unordered_map<std::string,ParseIni::IniValue>& ParseIni::operator[](const char* section_name){
    return _top_values[section_name];
}

}