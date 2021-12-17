#include <gflags/gflags.h>
#include <butil/logging.h>
#include <brpc/channel.h>


// #include "proto_test.pb.h"
#include "identity.pb.h"

#include "secure/rsa.h"

int main(int argc,char* argv[]){
    brpc::Channel channel;
    brpc::ChannelOptions options;
    options.protocol="h2:grpc";
    options.timeout_ms=100;
    options.max_retry=3;
    std::string server="127.0.0.1:5663";
    std::string load_balancer="";
    if(channel.Init(server.c_str(),load_balancer.c_str(),&options)!=0){
        std::cout<<"fail to initialize channel"<<std::endl;
        return -1;
    }

    blockchain::Identity_Stub stub(&channel);

    std::string pub_key;
    std::string pri_key;
    glib::generate_key(&pub_key,&pri_key);
    brpc::Controller cntl;
    blockchain::ApplyPubKeyCertRequest request;
    blockchain::ApplyPubKeyCertReply reply;
    request.set_host_name("gyy_host");
    request.set_pub_key(pub_key);
    stub.ApplyPubKeyCert(&cntl,&request,&reply,NULL);

    if(!cntl.Failed()){
        std::cout<<"host name: "<<reply.request().host_name()<<std::endl
            <<"host pub key: "<<reply.request().pub_key()<<std::endl
            <<"md5: "<<reply.request_md5()<<std::endl
            <<"signature: "<<reply.ca_signature()<<std::endl;
    }else{
        std::cout<<cntl.ErrorText()<<std::endl;
    }
    // while(!brpc::IsAskedToQuit()){
    //     brpc::Controller cntl;
    //     blockchain::CAPubKeyRequest request;
    //     blockchain::CAPubKeyReply reply;

    //     request.set_host_name("gyy_host");
    //     stub.GetCAPubKey(&cntl,&request,&reply,NULL);
    //     if(!cntl.Failed()){
    //         std::cout<<"Received pubkey: "<<reply.pub_key()<<std::endl;
    //     }else{
    //         std::cout<<cntl.ErrorText()<<std::endl;
    //     }
    //     usleep(1000*1000L);
    // }
    return 0;
}