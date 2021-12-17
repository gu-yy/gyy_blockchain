#ifndef SUBMIT_CONTRACT_H
#define SUBMIT_CONTRACT_H

#include "base_contract.h"

namespace store_node{
class submit_contract: public base_contract{
public:
    virtual std::vector<std::string> invoke(std::vector<std::string>& params);
    virtual std::vector<std::string> query(std::vector<std::string>& params);
};
}

#endif