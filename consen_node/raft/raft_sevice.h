#ifndef RAFT_SERVICE_H
#define RAFT_SERVICE_H

#include <unordered_set>
#include <vector>

#include "consen_node.pb.h"
#include "store_node.pb.h"

#include "raft.h"
#include "../init/consen_node_cache.h"
#include "../global/global.h"

namespace consen_node{
class raft_service_impl:public blockchain::ConsenNode{
public:
    raft_service_impl(){};
    ~raft_service_impl(){};
    void SendMsg(google::protobuf::RpcController* cntl_base,
                const blockchain::RpcMsg* req,
                blockchain::RpcMsg* rep,
                google::protobuf::Closure* done);
    void SendTxToConsenNode(google::protobuf::RpcController* cntl_base,
                            const blockchain::TxMsg* req,
                            blockchain::ConsenNodeResponse* rep,
                            google::protobuf::Closure* done);
    void GetTxFromFollower(google::protobuf::RpcController* cntl_base,
                            const blockchain::LeaderRequest* req,
                            blockchain::TxsFromFollower* rep,
                            google::protobuf::Closure* done);
    void SendBlockToOtherNode(google::protobuf::RpcController* cntl_base,
                            const blockchain::block* req,
                            blockchain::FollowerResponse* rep,
                            google::protobuf::Closure* done);           
};
}


#endif