#ifndef GOSSIP_SERVICE_H
#define GOSSIP_SERVICE_H

#include "store_node.pb.h"

#include "gossip.h"
#include "../global/global.h"

namespace store_node{

class gossip_service_impl:public blockchain::StoreNode{
public:
    gossip_service_impl(){};
    ~gossip_service_impl(){};
    void SendBlockToStoreNode(google::protobuf::RpcController* cntl_base,
                            const blockchain::block* req,
                            blockchain::BlockReceived* rep,
                            google::protobuf::Closure* done);
    void ReceivedBlockBefore(google::protobuf::RpcController* cntl_base,
                            const blockchain::NewBlockMsg* req,
                            blockchain::BlockReceived* rep,
                            google::protobuf::Closure* done);
};

}

#endif