#ifndef STORE_NODE_IDENTITY_H
#define STORE_NODE_IDENTITY_H

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <brpc/channel.h>


#include "identity.pb.h"

#include "../global/global.h"
#include "secure/rsa.h"

namespace store_node{
// 主要是用来初始化结点的身份信息，包括密钥等
class node_identity{
public:
    static node_identity* get_node_identity(){
        if(_node_identity==nullptr){
            _node_identity=new node_identity(global_ini);
        }
        return _node_identity;
    }
    blockchain::ApplyPubKeyCertReply get_cert(){return _cert;}
    std::string get_pub_key(){return _pub_key;}
    std::string get_pri_key(){return _pri_key;}
private:
    static node_identity* _node_identity;
    blockchain::ApplyPubKeyCertReply _cert;
    std::string _pub_key;
    std::string _pri_key;
    node_identity(glib::ParseIni& ini);
};

}


#endif