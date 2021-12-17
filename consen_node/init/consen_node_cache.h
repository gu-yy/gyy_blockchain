#ifndef CONSEN_NODE_CACHE_H
#define CONSEN_NODE_CACHE_H

#include <mutex>
#include <queue>
#include <unordered_set>

#include "consen_node.pb.h"
#include "block.pb.h"

#include "../raft/raft.h"
#include "../global/global.h"
#include "consen_node_identity.h"
#include "secure/md5.h"
#include "secure/signature.h"

namespace consen_node {
struct tx_node{
    tx_node(const blockchain::tx& tx):_tx(tx),_timestamp(tx.timestamp()),_next(nullptr),_prev(nullptr){}
    ~tx_node(){
        _next=nullptr;
        _next=nullptr;
    }
    uint64_t _timestamp;
    blockchain::tx _tx;
    tx_node* _next;
    tx_node* _prev;
};

class consen_node_cache{
public:
    static consen_node_cache* get_tx_cache(){
        if(_tx_cache==nullptr){
            _tx_cache=new consen_node_cache();
        }
        return _tx_cache;
    }
    consen_node_cache():_head(nullptr),_tail(nullptr),_tx_size(0){}
    ~consen_node_cache(){
        clear_cache();
    }
    void insert_tx(const blockchain::tx& tx);
    void save_store_node(const std::string& host_address){
        // ip+port
        _store_nodes.insert(host_address);
    }
    void remove_store_node(const std::string& host_address){
        _store_nodes.erase(host_address);
    }
    std::unordered_set<std::string> get_store_nodes(){
        return _store_nodes;
    }
    tx_node* get_all_tx();
    blockchain::block generate_block();
    void distribute_block();
    void clear_cache(){
        bool flag=true;
        do{
            if(_head==_tail){
                flag=false;
            }
            tx_node* tmp=_head->_next;
            delete _head;
            _head=tmp;
        }while(flag);
        _head=nullptr;
        _tail=nullptr;
    }
private:
    static consen_node_cache* _tx_cache;
    uint32_t _tx_size;
    tx_node* _head;
    tx_node* _tail;
    std::mutex _tx_mutex;

    std::unordered_set<std::string> _store_nodes;
};
}


#endif