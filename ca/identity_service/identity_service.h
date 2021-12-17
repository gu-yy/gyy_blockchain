#ifndef IDENTITY_SERVICE_H
#define IDENTITY_SERVICE_H

#include <string>
#include "identity.pb.h"
#include "brpc/server.h"

#include "secure/md5.h"
#include "secure/rsa.h"
#include "secure/signature.h"

namespace ca{
class CAIdentityServiceImpl: public blockchain::Identity{
public:
    CAIdentityServiceImpl(std::string& public_key,std::string& private_key):_pub_key(public_key),_pri_key(private_key){};
    ~CAIdentityServiceImpl(){};
    void GetCAPubKey(google::protobuf::RpcController* cntl_base,
                    const blockchain::CAPubKeyRequest* req,
                    blockchain::CAPubKeyReply* rep,
                    google::protobuf::Closure* done);
    void ApplyPubKeyCert(google::protobuf::RpcController* cntl_base,
                    const blockchain::ApplyPubKeyCertRequest* req,
                    blockchain::ApplyPubKeyCertReply* rep,
                    google::protobuf::Closure* done);
private:
    std::string _pub_key;
    std::string _pri_key;
};

}


#endif