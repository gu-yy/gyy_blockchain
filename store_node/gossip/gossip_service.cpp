#include "gossip_service.h"

namespace store_node{
void SendBlockToStoreNode(google::protobuf::RpcController* cntl_base,const blockchain::block* req,blockchain::BlockReceived* rep,google::protobuf::Closure* done){
    brpc::ClosureGuard done_guard(done);
    brpc::Controller* cntl=static_cast<brpc::Controller*>(cntl_base);
    // 1、将block保存到本地
    // 2、将block发送给其他store_node
}
void ReceivedBlockBefore(google::protobuf::RpcController* cntl_base,const blockchain::NewBlockMsg* req,blockchain::BlockReceived* rep,google::protobuf::Closure* done){
    brpc::ClosureGuard done_guard(done);
    brpc::Controller* cntl=static_cast<brpc::Controller*>(cntl_base);
    gossip* gossip_handler=gossip::get_gossip_handler();
    if(gossip_handler->get_expect_hight()==req->number() && gossip_handler->get_pre_hash()==req->pre_hash()){
        rep->set_has_received(false);
    }else{
        rep->set_has_received(true);
    }
}
}