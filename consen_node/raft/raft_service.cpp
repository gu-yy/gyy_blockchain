#include "raft_sevice.h"

namespace consen_node{
void raft_service_impl::SendMsg(google::protobuf::RpcController* cntl_base,const blockchain::RpcMsg* req,blockchain::RpcMsg* rep,google::protobuf::Closure* done){
    raft* raft_node=raft::get_raft_handler();
    brpc::ClosureGuard done_guard(done);
    brpc::Controller* cntl=static_cast<brpc::Controller*>(cntl_base);
    uint32_t type=req->type();
    if(type==0){
        // 这是心跳信息
        raft_node->reset_elapse_time();
    }else if(type==1){
        bool get_ticket=raft_node->vote(req->session());
        if(get_ticket){
            rep->set_aggree(true);
        }
    }
}

void raft_service_impl::SendTxToConsenNode(google::protobuf::RpcController* cntl_base,const blockchain::TxMsg* req,blockchain::ConsenNodeResponse* rep,google::protobuf::Closure* done){
    consen_node_cache* tx_cache=consen_node_cache::get_tx_cache();
    brpc::ClosureGuard done_guard(done);
    brpc::Controller* cntl=static_cast<brpc::Controller*>(cntl_base);
    tx_cache->insert_tx(req->tx_msg());
    tx_cache->save_store_node(req->host_address());
    rep->set_sueecss(true);
}

void raft_service_impl::GetTxFromFollower(google::protobuf::RpcController* cntl_base,const blockchain::LeaderRequest* req,blockchain::TxsFromFollower* rep,google::protobuf::Closure* done){
    raft* raft_node=raft::get_raft_handler();
    consen_node_cache* tx_cache=consen_node_cache::get_tx_cache();
    brpc::ClosureGuard done_guard(done);
    brpc::Controller* cntl=static_cast<brpc::Controller*>(cntl_base);
    if(LEADER!=raft_node->get_node_role()){
        tx_node* tx=tx_cache->get_all_tx();
        tx_node* tmp=tx;
        do{
            blockchain::tx* next_tx=rep->add_txs();
            next_tx=&tmp->_tx;
            tmp=tmp->_next;
        }while(tmp!=tx);
    }
    tx_cache->clear_cache();
}

void raft_service_impl::SendBlockToOtherNode(google::protobuf::RpcController* cntl_base,const blockchain::block* req,blockchain::FollowerResponse* rep,google::protobuf::Closure* done){
    raft* raft_node=raft::get_raft_handler();
    consen_node_cache* tx_cache=consen_node_cache::get_tx_cache();
    brpc::ClosureGuard done_guard(done);
    brpc::Controller* cntl=static_cast<brpc::Controller*>(cntl_base);
    // 用这个block修改本地的hight和hash
    raft_node->set_pre_hash(req->block_header().cur_hash());
    raft_node->set_current_hight(req->block_header().number()+1);
    // 调用store_node的rpc服务，将block发给他们
    // 异步
    std::unordered_set<std::string> store_nodes=tx_cache->get_store_nodes();
    size_t store_nodes_num=store_nodes.size();
    std::vector<std::string> store_nodes_vec(store_nodes_num);
    std::vector<brpc::Controller> controllers(store_nodes_num);
    std::vector<blockchain::BlockReceived> responses(store_nodes_num);
    size_t idx=0;
    for(std::string store_node_address:store_nodes){
        brpc::Channel channel;
        brpc::ChannelOptions options;
        options.protocol="h2:grpc";
        options.timeout_ms=100;
        options.max_retry=3;
        std::string load_balancer="";
        channel.Init(store_node_address.c_str(),load_balancer.c_str(),&options);
        store_nodes_vec[idx]=store_node_address;
        blockchain::StoreNode_Stub stub(&channel);
        stub.SendBlockToStoreNode(&controllers[idx],req,&responses[idx],brpc::DoNothing());
        idx++;
    }
    for(int i=0;i<idx;++i){
        brpc::Join(controllers[i].call_id());
        if(controllers[i].Failed()){
            tx_cache->remove_store_node(store_nodes_vec[i]);
        }
    }
} 
}