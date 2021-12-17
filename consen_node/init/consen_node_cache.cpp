#include "consen_node_cache.h"

namespace consen_node{

consen_node_cache* consen_node_cache::_tx_cache=nullptr;

void consen_node_cache::insert_tx(const blockchain::tx& tx){
    tx_node* new_tx=new tx_node(tx);
    std::lock_guard<std::mutex> lg(_tx_mutex);
    if(_head==nullptr){
        _head=new_tx;
        _tail=_head;
    }else{
        if(new_tx->_timestamp>_tail->_timestamp){
            new_tx->_prev=_tail;
            _tail->_next=new_tx;
            _tail=new_tx;
        }else if(new_tx->_timestamp<_head->_timestamp){
            new_tx->_next=_head;
            _head->_prev=new_tx;
            _head=new_tx;
        }else{
            tx_node* tmp=_tail;
            while(tmp->_timestamp>new_tx->_timestamp){
                tmp=tmp->_prev;
            }
            new_tx->_prev=tmp;
            new_tx->_next=tmp->_next;
            tmp->_next->_prev=new_tx;
            tmp->_next=new_tx;
        }
    }
    _tx_size+=1;
}

tx_node* consen_node_cache::get_all_tx(){
    raft* raft_node=raft::get_raft_handler();
    if(LEADER==raft_node->get_node_role()){
        // leader，会送所有非leader结点请求交易，然后打包成新的block
        std::vector<blockchain::TxsFromFollower> txs=raft_node->get_tx_from_all_follower();
        // 归并的方式将所有的交易插入到leader的缓存中，然后打包生成新的区块
        std::priority_queue<std::pair<uint64_t,int>,std::vector<std::pair<uint64_t,int>>,std::greater<std::pair<uint64_t,int>>> merge_que;
        int receive_num=txs.size();
        std::vector<int> tx_index(receive_num,0);
        for(int i=0;i<receive_num;++i){
            if(txs[i].txs_size()>0){
                blockchain::tx tx=txs[i].txs(0);
                uint64_t timestamp=tx.timestamp();
                std::pair<uint64_t,int> merge_node_time_idx(timestamp,i);
                merge_que.push(merge_node_time_idx);
            }
        }
        // 开始归并
        while(!merge_que.empty()){
            std::pair<uint64_t,int> first_tx_idx=merge_que.top();
            merge_que.pop();
            blockchain::tx tx=txs[first_tx_idx.second].txs(tx_index[first_tx_idx.second]);
            tx_index[first_tx_idx.second]+=1;
            insert_tx(tx);
            if(txs[first_tx_idx.second].txs_size()>tx_index[first_tx_idx.second]){
                uint64_t next_timestamp=txs[first_tx_idx.second].txs(tx_index[first_tx_idx.second]).timestamp();
                std::pair<uint64_t,int> next_merge_node(next_timestamp,first_tx_idx.second);
                merge_que.push(next_merge_node);
            }
        }
    }
    return _head;
}

blockchain::block consen_node_cache::generate_block(){
    blockchain::block new_block;
    node_identity* node_identity_handler=node_identity::get_node_identity();
    raft* raft_node=raft::get_raft_handler();
    if(LEADER==raft_node->get_node_role()){
        // 获取前一个区块的高度，如果新区块不是第一个区块，那么还要获取前一个区块的hash值
        std::string pre_hash=raft_node->get_pre_hash();
        uint32_t cur_hight=raft_node->get_current_hight();
        // 计算当前区块hash值方式为hash(prehash+hight+block_body)
        blockchain::body* new_body=new_block.mutable_block_body();
        tx_node* txs=get_all_tx();
        bool flag=true;
        do{
            if(txs==_tail){
                flag=false;
            }
            blockchain::tx* new_tx=new_body->add_txs();
            new_tx=&txs->_tx;
            txs=txs->_next;
        }while(flag);
        clear_cache();
        std::string seri_block=pre_hash+std::to_string(cur_hight)+new_body->SerializeAsString();
        std::string cur_hash=glib::md5(seri_block);
        blockchain::header* new_header=new_block.mutable_block_header();
        new_header->set_pre_hash(pre_hash);
        new_header->set_cur_hash(cur_hash);
        new_header->set_number(cur_hight);
        seri_block=seri_block+new_header->SerializeAsString();
        new_block.set_signature(glib::sign(seri_block,node_identity_handler->get_pub_key()));
        raft_node->set_current_hight(++cur_hight);
        raft_node->set_pre_hash(cur_hash);
    }
    return new_block;
}

void consen_node_cache::distribute_block(){
    raft* raft_node=raft::get_raft_handler();
    if(LEADER==raft_node->get_node_role() && _tx_size>0){
        blockchain::block block=generate_block();
        std::vector<std::string> failed_store_nodes=raft_node->send_block_to_follower(block,_store_nodes);
        for(std::string store_node_address:failed_store_nodes){
            remove_store_node(store_node_address);
        }
    }
}

}