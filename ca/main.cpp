#include <iostream>

#include "brpc/server.h"
#include "identity.pb.h"

#include "gini/gini.h"
#include "secure/md5.h"
#include "secure/signature.h"
#include "identity_service/identity_service.h"

int main(int argc,char* argv[]){
    // 加载配置文件
    glib::ParseIni ca_ini("./config/","ca_config.ini");
    std::string pub_key=ca_ini["identity"]["public_key"].to_string();
    std::string pri_key=ca_ini["identity"]["private_key"].to_string();

    brpc::Server server;
    ca::CAIdentityServiceImpl ca_svc(pub_key,pri_key);
    if(server.AddService(&ca_svc,brpc::SERVER_DOESNT_OWN_SERVICE)!=0){
        std::cout<<"fail to add ca_svc"<<std::endl;
        return -1;
    }

    brpc::ServerOptions options;
    options.idle_timeout_sec=-1;
    if(server.Start(5663,&options)!=0){
        std::cout<<"fail to start ca_svc"<<std::endl;
        return -1;
    }

    std::cout<<"hello gyy"<<std::endl;
    server.RunUntilAskedToQuit();
    return 0;
}
