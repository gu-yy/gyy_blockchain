#ifndef TRACE_CONTRACT_H
#define TRACE_CONTRACT_H

#include "base_contract.h"

namespace store_node{
class trace_contract: public base_contract{
public:
    virtual std::vector<std::string> invoke(std::vector<std::string>& params);
    virtual std::vector<std::string> query(std::vector<std::string>& params);
};
}

#endif