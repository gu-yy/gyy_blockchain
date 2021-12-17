#include "consen_node_identity.h"

namespace consen_node{

static int load_key(std::string pub_key_file,std::string pri_key_file,std::string* pub_key,std::string* pri_key){
    bool regenerate_key=false;
    if(0!=access(pub_key_file.c_str(),F_OK)){
        regenerate_key=true;
    }else{
        *pub_key=read_file_once(pub_key_file);
        if((*pub_key).size()==0){
            regenerate_key=true;
        }
    }
    if(0!=access(pri_key_file.c_str(),F_OK)){
        regenerate_key=true;
    }else{
        *pri_key=read_file_once(pri_key_file);
        if((*pri_key).size()==0){
            regenerate_key=true;
        }
    }
    if(regenerate_key){
        glib::generate_key(pub_key,pri_key);
        write_file_once(pub_key_file,(*pub_key).c_str(),(*pub_key).size());
        write_file_once(pri_key_file,(*pri_key).c_str(),(*pri_key).size());
        return 1;
    }
    return 0;
}

static std::string get_cert_from_ca(std::string& ca_server,std::string& host_name,std::string& pub_key){
    brpc::Channel channel;
    brpc::ChannelOptions options;
    options.protocol="h2:grpc";
    options.timeout_ms=100;
    options.max_retry=3;
    std::string load_balancer="";
    channel.Init(ca_server.c_str(),load_balancer.c_str(),&options);

    blockchain::Identity_Stub stub(&channel);
    brpc::Controller cntl;
    blockchain::ApplyPubKeyCertRequest request;
    blockchain::ApplyPubKeyCertReply reply;
    request.set_host_name(host_name);
    request.set_pub_key(pub_key);
    for(int i=0;i<7;++i){
        stub.ApplyPubKeyCert(&cntl,&request,&reply,NULL);
        if(!cntl.Failed()){
            return reply.SerializeAsString();
        }
    }
    return "";
}

node_identity::node_identity(glib::ParseIni& ini){
    std::string cert_file_path=ini["node_identity_message"]["identity_message_path"].to_string();
    if(cert_file_path.back()!='/'){
        cert_file_path.push_back('/');
    }
    std::string pub_key_file_name=ini["node_identity_message"]["pub_key_file"].to_string();
    std::string pri_key_file_name=ini["node_identity_message"]["pri_key_file"].to_string();
    std::string cert_file_name=ini["node_identity_message"]["cert_file"].to_string();

    if(0!=access(cert_file_path.c_str(),F_OK)){
        mk_multi_dir(cert_file_path);
    }
    
    std::string pub_key_file=cert_file_path+pub_key_file_name;
    std::string pri_key_file=cert_file_path+pri_key_file_name;
    int reget_cert=load_key(pub_key_file,pri_key_file,&_pub_key,&_pri_key);

    std::string cert_file=cert_file_path+cert_file_name;
    bool exist_cert=false;
    
    std::string cert_content;
    if(reget_cert==0){
        // 直接读取，前提是有
        if(0==access(cert_file.c_str(),F_OK)){
            cert_content=read_file_once(cert_content);
        }
    }
    if(cert_content.size()==0){
        // 重新向ca获取
        std::string ca_ip=ini["ca"]["ip"].to_string();
        std::string ca_port=ini["ca"]["port"].to_string();
        std::string ca_server=ca_ip+":"+ca_port;
        std::string host_name=ini["current_node"]["host_name"].to_string();
        cert_content=get_cert_from_ca(ca_server,host_name,_pub_key);
    }
    write_file_once(cert_file,cert_content.c_str(),cert_content.size());
    _cert.ParseFromString(cert_content);
}

}