#include "gossip.h"

namespace store_node{
gossip* gossip::_gossip=nullptr;
std::vector<std::string> gossip::unreceived_before(uint32_t number,std::string& pre_hash){
    std::vector<std::string> unreceived_block_nodes;
    size_t neighbor_node_num=_rpc_channels.size();
    std::vector<brpc::Controller> controllers(neighbor_node_num);
    std::vector<blockchain::BlockReceived> responses(neighbor_node_num);
    std::vector<std::string> nodes(neighbor_node_num);
    size_t idx=0;
    for(auto it=_rpc_channels.begin();it!=_rpc_channels.end();++it){
        nodes[idx]=it->first;
        blockchain::StoreNode_Stub stub(&(it->second));
        blockchain::NewBlockMsg new_block_msg;
        new_block_msg.set_number(number);
        new_block_msg.set_pre_hash(pre_hash);
        stub.ReceivedBlockBefore(&controllers[idx],&new_block_msg,&responses[idx],brpc::DoNothing());
        idx++;
    }
    for(int i=0;i<idx;++i){
        brpc::Join(controllers[i].call_id());
        if(controllers[i].Failed()){
            _other_nodes.erase(nodes[i]);
            _rpc_channels.erase(nodes[i]);
        }else{
            if(!responses[i].has_received()){
                unreceived_block_nodes.push_back(nodes[i]);
            }
        }
    }
    return unreceived_block_nodes;
}
}