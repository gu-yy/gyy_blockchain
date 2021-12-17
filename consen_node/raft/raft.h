#ifndef RAFT_H
#define RAFT_H

#include <ctime>
#include <cstdlib>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "brpc/channel.h"
#include "consen_node.pb.h"
#include "store_node.pb.h"

#include "../global/global.h"

namespace consen_node{
 
class raft{
public:
    static raft* get_raft_handler(){
        if(_raft==nullptr){
            size_t consen_node_num=global_ini["consen_node_hosts"].size();
            srand(time(NULL));
            time_t hearbeat_interval=global_ini["raft"]["time_interval"].to_int();
            time_t interval=rand()%consen_node_num+hearbeat_interval;
            _raft=new raft(interval,hearbeat_interval);
        }
        return _raft;
    }
    node_role get_node_role(){
        return _role;
    }
    std::string get_pre_hash(){
        return _pre_hash;
    }
    void set_pre_hash(std::string hash){
        _pre_hash=hash;
    }
    uint32_t get_current_hight(){
        return _cur_hight;
    }
    void set_current_hight(uint32_t hight){
        _cur_hight=hight;
    }

    // 每秒执行一次的循环执行函数
    void loop_process();
    // 如果是leader结点，那么就异步发送心跳信息
    void send_heartbeat();
    void reset_elapse_time(){
        _time_elapsed=0;
    }
    void reselection_leader();
    bool vote(uint32_t session);
    std::vector<blockchain::TxsFromFollower> get_tx_from_all_follower();
    std::vector<std::string> send_block_to_follower(const blockchain::block& block,const std::unordered_set<std::string>& store_nodes);    

private:
    static raft* _raft;
    int _ticket_num;
    uint32_t _session;
    bool _voted;
    std::time_t _time_elapsed;
    std::time_t _time_trigger;
    std::time_t _hearbeat_interval;
    node_role _role;
    std::unordered_map<std::string,std::string> _other_nodes;
    brpc::ChannelOptions _rpc_options;
    std::string _rpc_load_balancer;
    std::unordered_map<std::string,brpc::Channel> _rpc_channels;

    std::string _pre_hash;
    uint32_t _cur_hight;

    raft(time_t trigger_time,time_t heartbeat_time):_ticket_num(0),_session(0),_voted(false),_time_elapsed(0),_time_trigger(trigger_time),_hearbeat_interval(heartbeat_time),_role(FOLLOWER),_pre_hash(""),_cur_hight(0){
        _rpc_options.protocol="h2:grpc";
        _rpc_options.timeout_ms=100;
        _rpc_options.max_retry=3;
        for(auto it=global_ini["consen_node_hosts"].begin();it!=global_ini["consen_node_hosts"].end();++it){
            _other_nodes[it->first]=it->second.to_string();
            _rpc_channels[it->first].Init(it->second.to_string().c_str(),_rpc_load_balancer.c_str(),&_rpc_options);
        }
    };
};


}



#endif