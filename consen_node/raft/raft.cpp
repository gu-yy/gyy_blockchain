#include "raft.h"

namespace consen_node{
raft* raft::_raft=nullptr;

void raft::send_heartbeat(){
    size_t cur_node_num=_rpc_channels.size();
    std::vector<brpc::Controller> controllers(cur_node_num);
    std::vector<blockchain::RpcMsg> responses(cur_node_num);
    std::vector<std::string> nodes(cur_node_num);
    size_t idx=0;
    for(auto it=_rpc_channels.begin();it!=_rpc_channels.end();++it){
        nodes[idx]=it->first;
        blockchain::ConsenNode_Stub stub(&(it->second));
        blockchain::RpcMsg heartbeat;
        heartbeat.set_type(0);
        stub.SendMsg(&controllers[idx],&heartbeat,&responses[idx],brpc::DoNothing());
        idx++;
    }
    for(int i=0;i<idx;++i){
        brpc::Join(controllers[i].call_id());
        if(controllers[i].Failed()){
            _other_nodes.erase(nodes[i]);
            _rpc_channels.erase(nodes[i]);
        }
    }
}

void raft::reselection_leader(){
    if(_role==FOLLOWER){
        _role=CANDIDATE;
    }
    size_t other_node_num=_rpc_channels.size();
    std::vector<brpc::Controller> controllers(other_node_num);
    std::vector<blockchain::RpcMsg> responses(other_node_num);
    std::vector<std::string> nodes(other_node_num);
    size_t idx=0;
    for(auto it=_rpc_channels.begin();it!=_rpc_channels.end();++it){
        nodes[idx]=it->first;
        blockchain::ConsenNode_Stub stub(&(it->second));
        blockchain::RpcMsg vote_req;
        vote_req.set_type(1);
        stub.SendMsg(&controllers[idx],&vote_req,&responses[idx],brpc::DoNothing());
        idx++;
    }
    for(int i=0;i<idx;++i){
        brpc::Join(controllers[i].call_id());
        if(controllers[i].Failed()){
            _other_nodes.erase(nodes[i]);
            _rpc_channels.erase(nodes[i]);
        }else{
            if(responses[i].aggree()==true){
                _ticket_num+=1;
            }
        }
    }
    if(_ticket_num>_other_nodes.size()/2){
        _role=LEADER;
    }else{
        _role=FOLLOWER;
        _ticket_num=1;
    }
}

std::vector<blockchain::TxsFromFollower> raft::get_tx_from_all_follower(){
    if(_role!=LEADER){
        return std::vector<blockchain::TxsFromFollower>();
    }
    size_t other_node_num=_rpc_channels.size();
    std::vector<brpc::Controller> controllers(other_node_num);
    std::vector<blockchain::TxsFromFollower> responses(other_node_num);
    std::vector<std::string> nodes(other_node_num);
    size_t idx=0;
    for(auto it=_rpc_channels.begin();it!=_rpc_channels.end();++it){
        nodes[idx]=it->first;
        blockchain::ConsenNode_Stub stub(&(it->second));
        blockchain::LeaderRequest leader_get_txs_request;
        leader_get_txs_request.set_host_address(global_ini["current_node"]["host_name"].to_string());
        stub.GetTxFromFollower(&controllers[idx],&leader_get_txs_request,&responses[idx],brpc::DoNothing());
        idx++;
    }
    for(int i=0;i<idx;++i){
        brpc::Join(controllers[i].call_id());
        if(controllers[i].Failed()){
            _other_nodes.erase(nodes[i]);
            _rpc_channels.erase(nodes[i]);
        }
    }
    return responses;
}

std::vector<std::string> raft::send_block_to_follower(const blockchain::block& block,const std::unordered_set<std::string>& store_nodes){
    if(_role==LEADER){
        // 发给其他consen_node
        size_t other_node_num=_rpc_channels.size();
        std::vector<brpc::Controller> controllers(other_node_num);
        std::vector<blockchain::FollowerResponse> responses(other_node_num);
        std::vector<std::string> nodes(other_node_num);
        size_t idx=0;
        for(auto it=_rpc_channels.begin();it!=_rpc_channels.end();++it){
            nodes[idx]=it->first;
            blockchain::ConsenNode_Stub stub(&(it->second));
            stub.SendBlockToOtherNode(&controllers[idx],&block,&responses[idx],brpc::DoNothing());
            idx++;
        }
        for(int i=0;i<idx;++i){
            brpc::Join(controllers[i].call_id());
            if(controllers[i].Failed()){
                _other_nodes.erase(nodes[i]);
                _rpc_channels.erase(nodes[i]);
            }
        }
    }
    // 发给自己对应的store_node
    std::vector<std::string> failed_store_nodes;
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
        stub.SendBlockToStoreNode(&controllers[idx],&block,&responses[idx],brpc::DoNothing());
        idx++;
    }
    for(int i=0;i<idx;++i){
        brpc::Join(controllers[i].call_id());
        if(controllers[i].Failed()){
            failed_store_nodes.push_back(store_nodes_vec[i]);
        }
    }

    return failed_store_nodes;
}


bool raft::vote(uint32_t session){
    if(_role!=FOLLOWER){
        return false;
    }
    if(session>_session){
        _session=session;
        _voted=true;
        return true;
    }else if(session<_session){
        return false;
    }else{
        if(!_voted){
            _voted=true;
            return true;
        }
        return false;
    }
}

void raft::loop_process(){
    if(_role==LEADER){
        send_heartbeat();
    }else if(_role==CANDIDATE){
        if(_ticket_num>=_other_nodes.size()/2){
            _role=LEADER;
        }else{
            _ticket_num=1;
            _role=FOLLOWER;
        }
    }else{
        // FOLLOWER
        if(_time_elapsed>=_time_trigger){
            reselection_leader();
        }
        _time_elapsed+=1;
    }
}
}