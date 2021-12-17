#ifndef GOSSIP_H
#define GOSSIP_H

#include <unordered_map>
#include <vector>

#include "brpc/channel.h"
#include "store_node.pb.h"

#include "../global/global.h"

namespace store_node{

class gossip{
public:
    static gossip* get_gossip_handler(){
        if(_gossip==nullptr){
            _gossip=new gossip();
        }
        return _gossip;
    }

    uint32_t get_expect_hight(){
        return _expect_hight;
    }
    void set_expect_hight(uint32_t hight){
        _expect_hight=hight;
    }
    std::string get_pre_hash(){
        return _pre_hash;
    }
    void set_pre_hash(const std::string& hash){
        _pre_hash=hash;
    }

    std::vector<std::string> unreceived_before(uint32_t number,std::string& pre_hash);
    void send_block_to_other_store_node(const blockchain::block& block);

private:
    static gossip* _gossip;
    uint32_t _expect_hight;
    std::string _pre_hash;

    std::unordered_map<std::string,std::string> _other_nodes;
    brpc::ChannelOptions _rpc_options;
    std::string _rpc_load_balancer;
    std::unordered_map<std::string,brpc::Channel> _rpc_channels;
    gossip():_expect_hight(0),_pre_hash(""){
        _rpc_options.protocol="h2:grpc";
        _rpc_options.timeout_ms=100;
        _rpc_options.max_retry=3;
        for(auto it=global_ini["store_node_hosts"].begin();it!=global_ini["store_node_hosts"].end();++it){
            _other_nodes[it->first]=it->second.to_string();
            _rpc_channels[it->first].Init(it->second.to_string().c_str(),_rpc_load_balancer.c_str(),&_rpc_options);
        }
    }
};

}


#endif