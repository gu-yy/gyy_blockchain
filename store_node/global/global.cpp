#include "global.h"

namespace store_node{

std::string read_file_once(std::string filename){
    if(0!=access(filename.c_str(),F_OK)){
        return "";
    }else{
        std::ifstream file(filename);
        if(file.is_open()){
            file.seekg(0,std::ios::end);
            int content_length=file.tellg();
            file.seekg(0,std::ios::beg);
            char content[content_length];
            file.read(content,content_length);
            file.close();
            return std::string(content);
        }
    }
}

int write_file_once(std::string filename,const char* message,int length){
    std::ofstream file(filename);
    if(file.is_open()){
        file.write(message,length);
        file.close();
        return 0;
    }
    return -1;
}

int mk_multi_dir(std::string& path){
    if(path.back()!='/'){
        path.push_back('/');
    }
    int index=0;
    while(true){
        int find_idx=path.find('/');
        if(find_idx==-1){
            break;
        }
        index=find_idx+1;
        std::string sub_dir=path.substr(0,index);
        if(0!=access(sub_dir.c_str(),F_OK)){
            if(0!=mkdir(sub_dir.c_str(),0775)){
                return -1;
            }
        }
    }
    return 0;
}

glib::ParseIni global_ini("./config/","config.ini");
blockchain::ApplyPubKeyCertReply global_node_cert;
size_t global_blockfile_number=0;
size_t global_block_number=0;

}