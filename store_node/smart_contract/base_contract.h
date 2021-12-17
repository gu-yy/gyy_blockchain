# ifndef BASE_CONTRACT_H
# define BASE_CONTRACT_H

#include <vector>
#include <string>

namespace store_node{
// template<typename T1,typename T2>
// class contract{
// public:
//     T2 invoke(T1& params){
//         return params.invoke();
//     }
//     T2 query(T1& params){
//         return params.query();
//     }
// };
class base_contract{
public:
    virtual std::vector<std::string> invoke(std::vector<std::string>& params)=0;
    virtual std::vector<std::string> invoke(std::vector<std::string>& params)=0;
};
}
#endif