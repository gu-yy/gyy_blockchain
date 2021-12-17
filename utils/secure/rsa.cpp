#include "rsa.h"

namespace glib{
static void generate_prime(mpz_t& p,gmp_randstate_t state){
    mpz_urandomb(p,state,256);
    do{
        mpz_nextprime(p,p);
    }while(0==mpz_probab_prime_p(p,50));
}

void generate_key(std::string* pub_key,std::string* pri_key){
    time_t seed;
    time(&seed);
    gmp_randstate_t state;
    gmp_randinit_default(state);
    gmp_randseed_ui(state,seed);
    mpz_t p;
    mpz_t q;
    mpz_init(p);
    mpz_init(q);
    generate_prime(p,state);
    generate_prime(q,state);
    mpz_t n;
    mpz_init(n);
    mpz_mul(n,p,q);
    mpz_sub_ui(p,p,1);
    mpz_sub_ui(q,q,1);
    mpz_t r;
    mpz_init(r);
    mpz_mul(r,p,q);
    // 求e，直接将e设置为素数
    mpz_t e;
    mpz_init_set_ui(e,65537);
    // 计算d，满足de=1modr
    mpz_t d;
    mpz_init(d);
    mpz_invert(d,e,r);
    // gmp_printf("%s is %Zd\n","d",d);
    char inner_pub_key[512];
    gmp_sprintf(inner_pub_key,"n=%Zd|e=%Zd",n,e);
    char inner_pri_key[512];
    gmp_sprintf(inner_pri_key,"n=%Zd|d=%Zd",n,d);
    *pub_key=inner_pub_key;
    *pri_key=inner_pri_key;
}

static std::vector<mpz_class> parse_key(std::string& key){
    std::vector<mpz_class> keys;
    size_t separate=key.find('|');
    std::string first=key.substr(2,separate-2);
    std::string second=key.substr(separate+3,key.size()-separate-3);
    keys.push_back(mpz_class(first));
    keys.push_back(mpz_class(second));
    return keys;
}

static mpz_class serial_message(std::string& message){
    mpz_class num;
    for(char c:message){
        for(int i=0;i<8;++i){
            int tmp=1;
            tmp=tmp<<(7-i);
            num*=2;
            if((c&tmp)!=0){
                num+=1;
            }
        }
    }
    return num;
}

static std::string parse_message(mpz_class& num){
    std::string message;
    while(num!=0){
        char c=0;
        for(int i=0;i<8;++i){
            mpz_class bit=num%2;
            num/=2;
            if(bit==1){
                c=c|(1<<i);
            }
        }
        message.push_back(c);
    }
    std::reverse(message.begin(),message.end());
    return message;
}

std::string encrypt(std::string& message,std::string& key){
    std::vector<mpz_class> keys=parse_key(key);
    mpz_class plain_text=serial_message(message);
    mpz_t crypt;
    mpz_init(crypt);
    mpz_powm(crypt,plain_text.get_mpz_t(),keys[1].get_mpz_t(),keys[0].get_mpz_t());
    mpz_class C(crypt);
    std::string crypt_text=parse_message(C);
    return crypt_text;
}

std::string decrypt(std::string& message,std::string& key){
    std::vector<mpz_class> keys=parse_key(key);
    mpz_class crypt_text=serial_message(message);
    mpz_t decrypt;
    mpz_init(decrypt);
    mpz_powm(decrypt,crypt_text.get_mpz_t(),keys[1].get_mpz_t(),keys[0].get_mpz_t());
    mpz_class D(decrypt);
    std::string plain_text=parse_message(D);
    return plain_text;
}
}