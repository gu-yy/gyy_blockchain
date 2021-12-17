#include "signature.h"

namespace glib{

std::string sign(std::string text,std::string key){
    std::string md5_value=md5(text);
    std::string signature_value=encrypt(md5_value,key);
    return signature_value;
}
bool valid(std::string signature,std::string text,std::string key){
    std::string md5_value=md5(text);
    std::string valid_md5_value=decrypt(signature,key);
    return md5_value==valid_md5_value;
}

}