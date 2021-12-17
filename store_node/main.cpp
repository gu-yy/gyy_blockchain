#include <iostream>

#include "leveldb/db.h"
#include "brpc/server.h"
#include "gflags/gflags.h"

#include "test.h"
#include "init/statusdb.h"
#include "proto_test.pb.h"
#include "gini/gini.h"
#include "encode/encode.h"
#include "secure/rsa.h"
#include "secure/md5.h"
#include "secure/signature.h"
#include "init/store_node_identity.h"

class MyGreeterImpl:public helloworld::Greeter{
public:
    MyGreeterImpl(){};
    virtual ~MyGreeterImpl(){};
    void SayHello(google::protobuf::RpcController* cntl_base,
                const helloworld::HelloRequest* req,
                helloworld::HelloReply* res,
                google::protobuf::Closure* done){
        brpc::ClosureGuard done_guard(done);
        brpc::Controller* cntl=static_cast<brpc::Controller*>(cntl_base);
        res->set_message("Hello"+req->name());
    }
};

int main(int argc,char* argv[]){
    // leveldb::DB* db;
    // leveldb::Options options;
    // options.create_if_missing=true;
    // leveldb::Status status=leveldb::DB::Open(options,"/home/hust/final_gyy_tmp/testdb",&db);
    // assert(status.ok());
    // std::cout<<"hello world"<<std::endl;
    // int a=3;
    // int b=3;
    // int c=ggg(a,b);
    // std::cout<<c<<std::endl;
    // 初始化状态数据库
    // store_node::statusdb* statusdb=store_node::statusdb::get_statusdb();
    // 建立第一个brpc服务
    // brpc::Server server;
    // MyGreeterImpl http_svc;
    // if(server.AddService(&http_svc,
    //                         brpc::SERVER_DOESNT_OWN_SERVICE)!=0){
    //     std::cout<<"fail to add http_svc"<<std::endl;
    //     return -1;
    // }
    // brpc::ServerOptions options;
    // options.idle_timeout_sec=-1;
    // if(server.Start(4396,&options)!=0){
    //     std::cout<<"fail to start http server"<<std::endl;
    //     return -1;
    // }

    // brpc::Server server2;
    // MyGreeterImpl http_svc2;
    // if(server2.AddService(&http_svc2,
    //                         brpc::SERVER_DOESNT_OWN_SERVICE)!=0){
    //     std::cout<<"fail to add http_svc"<<std::endl;
    //     return -1;
    // }
    // brpc::ServerOptions options2;
    // options2.idle_timeout_sec=-1;
    // if(server2.Start(4397,&options2)!=0){
    //     std::cout<<"fail to start http server"<<std::endl;
    //     return -1;
    // }

    // server.RunUntilAskedToQuit();
    // server2.RunUntilAskedToQuit();
    // 加载ini配置文件
    // glib::ParseIni ini("./config/","config.ini");
    // std::cout<<ini["abc"]["aa"].to_string()<<std::endl;
    // std::cout<<ini["store_node_host"]["host2"].to_string()<<std::endl;
    // uint32_t a=3;
    // char b[4];
    // glib::EncodeInt2CharArray(b,a);
    // std::string pub_key;
    // std::string pri_key;
    // glib::generate_key(&pub_key,&pri_key);
    // std::cout<<pub_key<<std::endl<<pri_key<<std::endl;
    // std::string message="gyy like eat meat";
    // // glib::encrypt(message,pub_key);
    // // glib::decrypt(message,pri_key);
    // std::string crypt_text=glib::encrypt(message,pub_key);
    // std::cout<<crypt_text<<std::endl;
    // std::string plain_text=glib::decrypt(crypt_text,pri_key);
    // std::cout<<plain_text<<std::endl;
    // std::string message="gyy like eat meat";
    // std::string ss="ahefjkhesfarhekjfheaoifhaaaeiurhwekrhieshfiuhgdsifhiherfiuhenfdsgjothjoireghjlkrdhgiuehgaiutehwnroikfheiutyoiuewahfniuehgfiujadsbijfrsgfhiuewhfnkewfhiauwehgfianfkjdshfiuaehfiahfaihfeiuewhfeiuae";
    // std::string md5=glib::md5(message);
    // std::string sss=glib::md5(ss);
    // std::cout<<md5<<std::endl<<sss<<std::endl;
    // std::string pub_key;
    // std::string pri_key;
    // glib::generate_key(&pub_key,&pri_key);
    // std::cout<<pub_key<<std::endl<<pri_key<<std::endl;
    // std::string message="abc gyy cyl lol,aa,cccddd";
    // std::string my_signature=glib::sign(message,pri_key);
    // bool ans=glib::valid(my_signature,message,pub_key);
    // std::cout<<ans<<std::endl;
    // std::string message2="aaaa,ddd,ddd";
    // bool ans2=glib::valid(my_signature,message2,pri_key);
    // std::cout<<ans2<<std::endl;
    store_node::node_identity* identity=store_node::node_identity::get_node_identity();
    store_node::statusdb* db=store_node::statusdb::get_statusdb();
    return 0;
}