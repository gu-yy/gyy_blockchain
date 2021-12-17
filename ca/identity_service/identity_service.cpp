#include "identity_service.h"

namespace ca{

void CAIdentityServiceImpl::GetCAPubKey(google::protobuf::RpcController* cntl_base,const blockchain::CAPubKeyRequest* req,blockchain::CAPubKeyReply* rep,google::protobuf::Closure* done){
    brpc::ClosureGuard done_guard(done);
    brpc::Controller* cntl=static_cast<brpc::Controller*>(cntl_base);
    rep->set_pub_key(_pub_key);
}
void CAIdentityServiceImpl::ApplyPubKeyCert(google::protobuf::RpcController* cntl_base,const blockchain::ApplyPubKeyCertRequest* req,blockchain::ApplyPubKeyCertReply* rep,google::protobuf::Closure* done){
    brpc::ClosureGuard done_guard(done);
    brpc::Controller* cntl=static_cast<brpc::Controller*>(cntl_base);
    std::string client_pub_key=req->pub_key();
    std::string client_identity_message=req->SerializeAsString();
    std::string client_identity_message_md5=glib::md5(client_identity_message);
    std::string ca_signature=glib::sign(client_identity_message_md5,_pri_key);
    blockchain::ApplyPubKeyCertRequest* client_message=rep->mutable_request();
    client_message->set_host_name(req->host_name());
    client_message->set_pub_key(req->pub_key());
    rep->set_request_md5(client_identity_message_md5);
    rep->set_ca_signature(ca_signature);
}
}